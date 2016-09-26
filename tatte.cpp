/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility  whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

/* NOTE:  The implementation of the functions in this source file
 * is subject to change at anytime.  In fact, the implementations WILL likely
 * change, and developers do not need to worry themselves with the details of
 * how the functions are implemented.
 */
#include <memory>
#include <tatte.h>

/*********************************
 * TattooRep class definition
 *********************************/
TattE::TattooRep::TattooRep() :
	tattooTemplate(nullptr), templateSize(0) {
}

std::shared_ptr<uint8_t>
TattE::TattooRep::resizeTemplate(uint64_t size) {
	if (size > 0) {
		uint8_t *data = new uint8_t[size];
		/*
		 * std::default_delete partial specialization for array types so 
		 * delete[] will be called on destruction
		 * */
		tattooTemplate.reset(data, std::default_delete<uint8_t[]>());
		templateSize = size;
	} else {
		tattooTemplate = nullptr;
		templateSize = 0;
	}
	return tattooTemplate;
}

void
TattE::TattooRep::addBoundingBox(const TattE::BoundingBox &bb) {
	boundingBoxes.push_back(bb);
}

std::shared_ptr<uint8_t>
TattE::TattooRep::getTattooTemplatePtr() const {
	return tattooTemplate;
}

uint64_t
TattE::TattooRep::getTemplateSize() const {
	return templateSize;
}

std::vector<TattE::BoundingBox>
TattE::TattooRep::getBoundingBoxes() const {
	return boundingBoxes;
}
