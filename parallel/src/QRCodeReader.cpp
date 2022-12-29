#include "QRCodeReader.hpp"

#include <math.h>
#include <array>
#include <algorithm>
#include <sstream>
#include <omp.h>

#include "processing/ConnectedComponents.hpp"
#include "utils/Utils.hpp"
#include "utils/Vector2D.hpp"

/* DEBUG */
// #define DEBUG_QR_BINARY
#define CORRECT_BINARY "010110111011011010100010100001101011101111101111111101100010010101001010000100111101111110110010101110001000000101000010101000110110110010010011111110100000111011000010010110111011010110010110001110001001001010100011110010111110010000111110111011001"

struct QRPos
{
	Vector2D ul;
	Vector2D ur;
	Vector2D ll;
	Vector2D lr;
	Vector2D rightVec;
	Vector2D downVec;
	Vector2D diagonalVec;
	Vector2D center;
	bool deformed;
};

double getCenterDist(BoundingBox bbox_a, BoundingBox bbox_b)
{
	double delta_x = bbox_b.center_x - bbox_a.center_x;
	double delta_y = bbox_b.center_y - bbox_a.center_y;

	return sqrt(delta_x*delta_x + delta_y*delta_y);
}

bool isContained(BoundingBox bbox_a, BoundingBox bbox_b)
{
	return bbox_a.left >= bbox_b.left && bbox_a.right <= bbox_b.right && bbox_a.top >= bbox_b.top && bbox_a.bottom <= bbox_b.bottom;
}

std::array<BoundingBox,3> getPositioningBlocks(std::map<int, BoundingBox> bboxes )
{
	int idx = 0;
	std::array<BoundingBox,3> positioningBlocks;
	std::vector<BoundingBox> bboxesList = Utils::getMapValues<int,BoundingBox>(bboxes);

	for (int i = 0; i < bboxesList.size(); i++)
	{
		for (int j = i + 1; j < bboxesList.size(); j++)
		{
			BoundingBox bbox_a = bboxesList[i];
			BoundingBox bbox_b = bboxesList[j];

			if (getCenterDist(bbox_a, bbox_b) < COMPONENT_CENTER_DIST_MARGIN)
			{
				bool a_in_b = isContained(bbox_b, bbox_a);
				bool b_in_a = isContained(bbox_a, bbox_b);

				if(idx == 3 && (a_in_b || b_in_a))
					throw MORE_THAN_THREE_POSITIONING_BLOCKS_EXCPETION;

				if (a_in_b)
					positioningBlocks[idx++] = bbox_a;
				else if (b_in_a)
					positioningBlocks[idx++] = bbox_b;
			}
		}
	}

	if (idx != 3)
		throw LESS_THAN_THREE_POSITIONING_BLOCKS_EXCPETION;

	return positioningBlocks;
}

Image* cropImage(std::array<BoundingBox,3> positioningBlocks, Image* img)
{
	const int PADDING = 2;

	int left = std::min(std::min(positioningBlocks[0].left, positioningBlocks[1].left), positioningBlocks[2].left) - PADDING;
	int right = std::max(std::max(positioningBlocks[0].right, positioningBlocks[1].right), positioningBlocks[2].right) + PADDING;
	int top = std::min(std::min(positioningBlocks[0].top, positioningBlocks[1].top), positioningBlocks[2].top) - PADDING;
	int bottom = std::max(std::max(positioningBlocks[0].bottom, positioningBlocks[1].bottom), positioningBlocks[2].bottom) + PADDING;

	int longestDim = std::max((right - left), (bottom - top));

	Image* croppedImg = new Image(longestDim, longestDim);

	for(int y = 0; y < croppedImg->height; y++)
	{
		for(int x = 0; x < croppedImg->width; x++)
		{
			croppedImg->pixels[y][x].r = img->pixels[y+top][x+left].r;
			croppedImg->pixels[y][x].g = img->pixels[y+top][x+left].g;
			croppedImg->pixels[y][x].b = img->pixels[y+top][x+left].b;
		}
	}

	return croppedImg;
}

QRPos getQRPositioningRec(std::array<BoundingBox, 3> positioningBlocks, double margin, bool deformed, int depth)
{
	if (depth == 4)
		throw MAX_RECURSION_DEPTH_EXCEPTION;

	for(int i = 0; i < 3; i++)
	{
		Vector2D v1(
			positioningBlocks[(i + 1) % 3].center_x - positioningBlocks[i].center_x,
			positioningBlocks[(i + 1) % 3].center_y - positioningBlocks[i].center_y
		);

		Vector2D v2(
			positioningBlocks[(i + 2) % 3].center_x - positioningBlocks[i].center_x,
			positioningBlocks[(i + 2) % 3].center_y - positioningBlocks[i].center_y
		);

		if(abs(v1.norm() - v2.norm()) < margin)
		{
			Vector2D diagonalVec = v1 + v2;
			Vector2D ul(positioningBlocks[i].center_x, positioningBlocks[i].center_y);
			Vector2D center =  ul + (diagonalVec * 0.5);
			Vector2D rightVec;
			Vector2D downVec;

			if (v1.crossProd(v2) > 0)
			{
				rightVec = v1;
				downVec = v2;
			}
			else
			{
				rightVec = v2;
				downVec = v1;
			}

			Vector2D ur = ul + rightVec;
			Vector2D ll = ul + downVec;
			Vector2D lr = ll + rightVec;

			return QRPos
			{ 
				.ul = ul, .ur = ur, .ll = ll, .lr = lr, 
				.rightVec = rightVec, 
				.downVec = downVec, 
				.diagonalVec = diagonalVec, 
				.center = center, 
				.deformed = deformed
			};
		}
	}

	return getQRPositioningRec(positioningBlocks, margin*2, true, depth++);

	//throw new Exception("Error: Couldn't find diagonal");
}

QRPos getQRPositioning(std::array<BoundingBox, 3> positioningBlocks)
{
	return getQRPositioningRec(positioningBlocks, POSITIONING_BLOCKS_DIST_MARGIN, false, 1);
}

pcomp moduleValue(Image* img, int left, int right, int top, int bottom)
{
	/*
		Determine module value by calculating the mode in the
		inner section of the module (only taking into account
		the inner half of the square of pixels. This reduces
		the interference of neighbouring modules).
	*/

	int zeroCount = 0;
	int oneCount = 0;

	int ystart = top + (bottom-top)/4.0 + 1;
	int yend = bottom - (bottom-top)/4.0 + 1;

	int xstart = left + (right-left)/4.0 + 1;
	int xend = right - (right-left)/4.0 + 1;

	for (int y = ystart; y <= yend; y++)
	{
		for(int x = xstart; x <= xend; x++)
		{
			if(img->pixels[y][x].r == 0)
				zeroCount++;
			else
				oneCount++;
		}
	}

	return (oneCount > zeroCount ? 1 : 0);
}

std::string getBinaryCode(Image* img, int left, int right, int top, int bottom)
{
	std::ostringstream os;

	// OMP for -> join os
	for (int y = 0; y < 21; y++)
	{
		for (int x = 0; x < 21; x++)
		{
			if (
				!(x <= 7 && y <= 7) &&
				!(x <= 7 && y >= 13) &&
				!(x >= 13 && y <= 7)
			)
			{
				if( moduleValue(
					img,
					(int)round(left + x * (right - left) / 21.0),
					(int)round(left + (x+1) * (right - left) / 21.0),
					(int)round(top + y * (bottom-top)/21.0),
					(int)round(top + (y+1) * (bottom - top) / 21.0)
				) == 0)
					os << "1";
				else
					os << "0";

				#ifdef DEBUG_QR_BINARY
					bool failed = false;
					std::string correct = CORRECT_BINARY;

					std::string str = os.str();

					if (str[str.length() - 1] != correct[str.length() - 1])
					{
						// std::cout << "failed at x: " << x << " y: " << y << std::endl;
						// std::cout << "value: " << str[str.length() - 1] << std::endl;
						// std::cout << std::endl;
						failed = true;
					}

					int real_y = top + y * (bottom-top) / 21.0;
					int real_x = left + x * (right - left) / 21.0;
					Pixel* p = &img->pixels[real_y][real_x];

					if (failed)
					{
						p->r = 255;
						p->g = 0;
						p->b = 0;
					}
					else
					{
						p->r = 0;
						p->g = 255;
						p->b = 0;
					}
				#endif
			}
		}
	}

	return os.str();
}

std::string QRCodeReader::read(Image* img)
{
	int left = 0, top = 0, right = 0, bottom = 0;
	Image* auxImg = img->copy();


	/* Binarize image */

	ImageProcessor::convertToBW(auxImg, std::min<pcomp>(100, ImageProcessor::getOtsuTheshold(auxImg)));

	/* Run connected components algorithm */

	Labels* labels = ConnectedComponents::getLabelsParallel(auxImg);
	std::map<int, BoundingBox> bboxes = ConnectedComponents::getBoundingBoxes(labels);
	try
	{
		/* Find positioning blocks */

		std::array<BoundingBox,3> positioningBlocks = getPositioningBlocks(bboxes);

		/*
			Recalculate otsu threhsold, only taking into account pixels
			in the region of the QR code. Convert original image to black
			and white using this threshold  
		*/
		Image* croppedImage = cropImage(positioningBlocks, img);
		pcomp threshold = ImageProcessor::getOtsuTheshold(croppedImage);
		croppedImage->destroy();
		delete croppedImage;
		if(abs(100-threshold) > 50) threshold = 100;
		ImageProcessor::convertToBW(img, threshold);

		/* Get QR code positioning based on the found positioning blocks */

		QRPos qrpos = getQRPositioning(positioningBlocks);
		QRPos qrposTransformed = qrpos;

		Image* copy = img->copy();

		/* Get rotation angle (so that the QR code is correctly positioned) and rotate image */

		double angle = qrpos.rightVec.angleTo(Vector2D(1,0));
		ImageProcessor::rotationAroundPoint(img, copy, angle, qrpos.center);
		qrposTransformed.ul = qrpos.ul.rotateAroundPoint(angle, qrpos.center);
		qrposTransformed.ur = qrpos.ur.rotateAroundPoint(angle, qrpos.center);
		qrposTransformed.ll = qrpos.ll.rotateAroundPoint(angle, qrpos.center);
		qrposTransformed.lr = qrpos.lr.rotateAroundPoint(angle, qrpos.center);
		qrposTransformed.rightVec = qrposTransformed.ur - qrposTransformed.ul;
		qrposTransformed.downVec = qrposTransformed.ll - qrposTransformed.ul;
		qrposTransformed.diagonalVec = qrposTransformed.lr - qrposTransformed.ul;
		//qrposTransformed.center = qrposTransformed.center;


		if(qrpos.deformed)
		{
			/* Perform shear */

			copy->copyPixels(img);
			double x_shear = -qrposTransformed.downVec.x / qrposTransformed.downVec.y;
			ImageProcessor::shear(img, copy, x_shear, 0);
			qrposTransformed.ul = qrposTransformed.ul.shear(x_shear, 0);
			qrposTransformed.ur = qrposTransformed.ur.shear(x_shear, 0);
			qrposTransformed.ll = qrposTransformed.ll.shear(x_shear, 0);
			qrposTransformed.lr = qrposTransformed.lr.shear(x_shear, 0);
			qrposTransformed.rightVec = qrposTransformed.ur - qrposTransformed.ul;
			qrposTransformed.downVec = qrposTransformed.ll - qrposTransformed.ul;
			qrposTransformed.diagonalVec = qrposTransformed.lr - qrposTransformed.ul;
			//qrposTransformed.center = ...;
		}

		copy->destroy();
		delete copy;

		double positioningBlocksDistance = qrposTransformed.rightVec.norm();
		double moduleSize = positioningBlocksDistance / 14.0;

		left = (int)round(qrposTransformed.ul.x - 3.5 * moduleSize);
		right = (int)round(qrposTransformed.ul.x + 17.5 * moduleSize);
		top = (int)round(qrposTransformed.ul.y - 3.5 * moduleSize);
		bottom = (int)round(qrposTransformed.ll.y + 3.5 * moduleSize);
	}
	catch(const char* msg)
	{
		std::cerr << "Exception: " << msg << std::endl;
	}

	labels->destroy();
	delete labels;
	auxImg->destroy();
	delete auxImg;

	return (left != right && bottom != top) ? getBinaryCode(img, left, right, top, bottom) : "";
}