/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

/*
 * Defines data types and functions as described in the Tatt-E "Concept,
 * Evaluation Plan, and API" available at
 * https://nigos.nist.gov/tatt-e/tatt-e_api.pdf 
 */

#ifndef TATTE_H_
#define TATTE_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace TattE {

/**
 * @brief
 * Struct representing a single image
 */
typedef struct Image {
	/**
	 * @brief
	 * Labels describing the image type
	 */
	enum class ImageType {
		/** Tattoo image */
		Tattoo = 0,
		/** Sketch of tattoo */
		Sketch = 1,
		/** Unknown */
		Unknown = 2
	};

	/** @brief Number of pixels horizontally */
	uint16_t width;
	/** @brief Number of pixels vertically */
	uint16_t height;
	/** @brief Number of bits per pixel. Legal values are 8 and 24. */
	uint8_t depth;
	/** @brief Label describing the type of image */
	ImageType imageType;
	/** @brief managed pointer to raster scanned data.
	 * Either RGB color or intensity.
	 * If image_depth == 24 this points to  3WH bytes  RGBRGBRGB...
	 * If image_depth ==  8 this points to  WH bytes  IIIIIII */
	std::shared_ptr<uint8_t> data;

	Image() :
		width{0},
		height{0},
		depth{24},
		imageType{ImageType::Tattoo}
		{}

	Image(
		uint16_t width,
		uint16_t height,
		uint8_t depth,
		std::shared_ptr<uint8_t> &data,
		ImageType type
		) :
		width{width},
		height{height},
		depth{depth},
		data{data},
		imageType{type}
		{}

	/** @brief This function returns the size of the image data. */
	size_t
	size() const { return (width * height * (depth / 8)); }
} Image;

/**
 * @brief
 * Labels describing the type/role of the template
 * to be generated (provided as input to template generation)
 */
enum class TemplateRole {
	/** Enrollment template used to enroll into gallery */
	Enrollment = 0,
	/** Identification template used for search */
	Identification = 1
};

/**
 * @brief
 * Structure for bounding box around a detected tattoo
 *
 * @param x
 * X-coordinate of top-left corner of bounding box around tattoo
 * @param y
 * Y-coordinate of top-left corner of bounding box around tattoo
 * @param width
 * Width, in pixels, of bounding box around tattoo
 * @param height
 * Height, in pixels, of bounding box around tattoo
 * @param confidence
 * Certainty that this region contains a tattoo.
 * The higher the value, the more certain.
 */
typedef struct BoundingBox {
	/** @brief X-coordinate of top-left corner of bounding box around tattoo */
	uint16_t x;
	/** @brief Y-coordinate of top-left corner of bounding box around tattoo */
	uint16_t y;
	/** @brief Width, in pixels, of bounding box around tattoo */
	uint16_t width;
	/** @brief Height, in pixels, of bounding box around tattoo */
	uint16_t height;

	BoundingBox() :
		x{0},
		y{0},
		width{0},
		height{0}
		{}

	BoundingBox(
		uint16_t x,
		uint16_t y,
		uint16_t width,
		uint16_t height
		) :
		x{x},
		y{y},
		width{width},
		height{height}
		{}
} BoundingBox;

/**
 * @brief
 * Return codes for the functions
 * specified by this API
 */
enum class ReturnCode {
	/** Success */
	Success = 0,
	/** Error reading configuration files */
	ConfigError = 1,
	/**
	 * Image type, e.g., sketches, is not supported by
	 * the implementation
	 */
	ImageTypeNotSupported = 2,
	/** Elective refusal to process the input */
	RefuseInput = 3,
	/** Involuntary failure to process the image */
	ExtractError = 4,
	/** Cannot parse the input data */
	ParseError = 5,
	/**
	 * Elective refusal to produce a template
	 */
	TemplateCreationError = 6,
	/**
	 * An operation on the enrollment directory
	 * failed (e.g. permission, space)
	 */
	EnrollDirError = 7,
	/** The implementation cannot support the number of input images */
	NumDataError = 8,
	/**
	 * One or more template files are in an incorrect
	 * format or defective
	 */
	TemplateFormatError = 9,
	/**
	 * Cannot locate the input data - the input
	 * files or names seem incorrect
	 */
	InputLocationError = 10,
	/** Vendor-defined failure */
	VendorError = 11,
	/** Function is not implemented */
    NotImplemented = 12
};

/**
 * @brief
 * A structure to contain information about a failure by the software
 * under test.
 *
 * @details
 * An object of this class allows the software to return some information
 * from a function call. The string within this object can be optionally
 * set to provide more information for debugging etc. The status code
 * will be set by the function to Success on success, or one of the
 * other codes on failure.
 */
typedef struct ReturnStatus {

	ReturnStatus() {}
	/**
	 * @brief
	 * Create a ReturnStatus object.
	 *
	 * @param[in] code
	 * The return status code; required.
	 * @param[in] info
	 * The optional information string.
	 */
	ReturnStatus(
		const ReturnCode codein,
		const std::string &infoin = ""
		) :
		code(codein),
		info(infoin)
		{}

	/** @brief Return status code */
	ReturnCode code;

	/** @brief Optional information string */
	std::string info;

} ReturnStatus;

/**
 * @brief
 * Labels describing the composition of the 1:N gallery
 * (provided as input into gallery finalization function)
 */
enum class GalleryType {
	/** Consolidated, subject-based enrollment */
    Consolidated = 0,
    /** Unconsolidated, event-based or photo-based enrollment */
    Unconsolidated = 1
};

/**
 * @brief
 * Data structure for result of an identification search
 */
typedef struct Candidate {
	/** @brief If the candidate is valid, this should be set to true. If 
	 * the candidate computation failed, this should be set to false. */
	bool isAssigned;

	/** @brief The template ID from the enrollment database manifest */
	std::string templateId;

	/** @brief Measure of similarity between the identification template 
	 * and the enrolled candidate.  Higher scores mean more likelihood that 
	 * the samples are of the same person.  An algorithm is free to assign 
	 * any value to a candidate.
	 * The distribution of values will have an impact on the appearance of a 
	 * plot of false-negative and false-positive identification rates. */
	double similarityScore;

	Candidate() :
		isAssigned(false),
		templateId(""),
		similarityScore(-1.0)
		{}

	Candidate(
		bool assigned,
		std::string id,
		double score) :
		isAssigned(assigned),
		templateId(id),
		similarityScore(score)
		{}
} Candidate;

/* API functions to be implemented */

/**
 * @brief The interface to Tatt-E implementations.
 *
 * @details The submission software under test will implement 
 * this interface by subclassing this class and implementing each method 
 * therein.
 */
class Interface {
public:
	virtual ~Interface() {}

	/** @brief REQUIRED.  This function initializes the implementation under test and sets 
	 * all needed parameters for template creation.
	 *
	 * @details This function will be called N=1 times by the NIST application,
	 * prior to parallelizing M >= 1 calls to createTemplate() via fork().
	 *
	 * @param[in] configDir
	 * A read-only directory containing any developer-supplied configuration 
	 * parameters or run-time data files.
    * @param[in] role
     * A value from the TemplateRole enumeration that indicates the intended
     * usage of the template to be generated.  In this case, either a 1:N
     * enrollment template used for gallery enrollment or 1:N identification
     * template used for search.
	 */
	virtual ReturnStatus
	initializeTemplateCreation(
		const std::string &configDir,
		TemplateRole role) = 0;

	/**
	 * @brief REQUIRED.  This function takes an input vector of tattoos and outputs a template, 
	 * bounding boxes, and quality values. 
	 *
	 * @details For enrollment templates: If the function
	 * executes correctly (i.e. returns a successful exit status), the NIST 
	 * calling application will store the template.  The NIST application will 
	 * concatenate the templates and pass the result to the enrollment 
	 * finalization function.  When the implementation fails to produce a 
	 * template, it shall still return a blank template (which can be zero 
	 * bytes in length). The template will be included in the
	 * enrollment database/manifest like all other enrollment templates but 
	 * is not expected to contain any feature information.
	 * <br>For identification templates: If the function returns a 
	 * non-successful return status, the output template will not be used 
	 * in subsequent search operations.
	 *
	 * @param[in] tattoos
	 * A vector of the same tattoo from the same person.  Implementations must alter 
	 * their behavior according to the type and number of images/type of 
	 * image contained in the structure.  The input image type could be a 
	 * tattoo or a sketch image.
	 * The vector of tattoos will always contain the same type of imagery, 
	 * i.e., no mixing of tattoos and sketch images will occur.  
	 * <b>Note that implementation support for
	 * sketch images is OPTIONAL.  Implementation shall return
	 * TattE::ImageType::ImageTypeNotSupported if they do not support sketch
	 * images.  All algorithms must support tattoo images. </b>
	 * @param[in] role
	 * A value from the TemplateRole enumeration that indicates the intended 
	 * usage of the template to be generated.  In this case, either an 
	 * enrollment template used for gallery enrollment or an identification 
	 * template used for search.
     * @param[out] templ
     * The output template.  The format is entirely unregulated.  This will be
     * an empty vector when passed into the function, and the implementation can
     * resize and populate it with the appropriate data.
	 * @param[out] boundingBoxes
	 * A vector of vector of bounding boxes, one set of bounding boxes for each
	 * input image. This will be an empty vector when passed into this function.
     * For each image in the input vector, the function shall provide the 
	 * bounding box coordinates for tattoos found in each image. If there are 2 images in the 
	 * input vector, then the size of boundingBoxes shall be 2, that is, boundingBoxes[i] 
	 * is associated with tattoos[i].  If 3 spatially separate tattoos are found in tattoos[0]
	 * and 1 tattoo is found in tattoos[1], then the size of boundingBoxes[0] shall be 3, and
	 * the size of boundingBoxes[1] shall be 1.
	 * @param[out] quality
	 * A vector of quality values, one for each input image.  
	 * This will be an empty vector when passed into this function, and 
	 * the implementation shall populate a quality value corresponding to each 
	 * input image.  quality[i] shall correspond to tattoos[i].
	 * A measure of tattoo quality on [0,1] is indicative of expected utility to
	 * the matcher, or matchability.  This value could measure overall tattoo
	 * distinctiveness/information richness and would be an indicator of
	 * how well the tattoo would be expected to match.
	 * A value of 1 indicates high quality and that the tattoo would be expected
	 * to match well, and a value of 0 indicates low quality indicative that
	 * tattoo would not match well.
	 */
	virtual ReturnStatus
	createTemplate(
		const std::vector<Image> &tattoos,
		const TemplateRole &role,
		std::vector<uint8_t> &templ,
		std::vector<std::vector<BoundingBox>> &boundingBoxes,
		std::vector<double> &quality) = 0;

	/**
	 * @brief REQUIRED.  This function will be called after all enrollment templates have 
	 * been created and freezes the enrollment data.
	 * After this call the enrollment dataset will be forever read-only.
	 *
	 * @details This function allows the implementation to conduct,
	 * for example, statistical processing of the feature data, indexing and 
	 * data re-organization.  The function may create its own data structure.  
	 * It may increase or decrease the size of
	 * the stored data.  No output is expected from
	 * this function, except a return code.  The function will generally be 
	 * called in a separate process after all
	 * the enrollment processes are complete.
	 * NOTE: Implementations shall not move the input data.  Implementations 
	 * shall not point to the input data.
	 * Implementations should not assume the input data would be readable 
	 * after the call.  Implementations must,
	 * <b>at a minimum, copy the input data</b> or otherwise extract what is 
	 * needed for search.
	 *
     * @param[in] configDir
     * A read-only directory containing any developer-supplied configuration
	 * @param[in] enrollmentDir
	 * The top-level directory in which enrollment data was placed. This 
	 * variable allows an implementation
	 * to locate any private initialization data it elected to place in the 
	 * directory.
	 * @param[in] edbName
	 * The name of a single file containing concatenated templates, i.e. the 
	 * EDB described in <em>Data Structures Supporting the API</em>.
	 * While the file will have read-write-delete permission, the implementation
	 * should only alter the file if it preserves the necessary content, in 
	 * other files for example.
	 * The file may be opened directly.  It is not necessary to prepend a 
	 * directory name.  This is a NIST-provided
	 * input - implementers shall not internally hard-code or assume any values.
	 * @param[in] edbManifestName
	 * The name of a single file containing the EDB manifest described in 
	 * <em>Data Structures Supporting the API</em>.
	 * The file may be opened directly.  It is not necessary to prepend a 
	 * directory name.  This is a NIST-provided
	 * input - implementers shall not internally hard-code or assume any values.
     * @param[in] galleryType
     * The composition of the gallery as enumerated by GalleryType.
	 */
	virtual ReturnStatus
	finalizeEnrollment(
		const std::string &configDir,
		const std::string &enrollmentDir,
		const std::string &edbName,
		const std::string &edbManifestName,
		const GalleryType &galleryType) = 0;

	/** @brief REQUIRED.  This function will be called once prior to one or more calls to 
	 * identifyTemplate.  The function might set static internal variables 
	 * so that the enrollment database is available to the subsequent 
	 * identification searches.
	 *
	 * @param[in] configDir
	 * A read-only directory containing any developer-supplied configuration 
	 * parameters or run-time data files.
	 * @param[in] enrollmentDir
	 * The read-only top-level directory in which enrollment data was placed.
	 */
	virtual ReturnStatus
	initializeIdentification(
		const std::string &configDir,
		const std::string &enrollmentDir) = 0;

	/** @brief REQUIRED.  This function searches an identification template against the 
	 * enrollment set, and outputs a
	 * vector containing candidateListLength Candidates.
	 *
	 * @details Each candidate shall be populated by the implementation
	 * and added to candidateList.  Note that candidateList will be an empty 
	 * vector when passed into this function.  The candidates shall appear in 
	 * descending order of similarity score - i.e. most similar entries appear 
	 * first.
	 *
	 * @param[in] idTemplate
	 * A template from createTemplate().  If the value returned by that 
	 * function was non-successful, the contents of idTemplate will not be 
	 * used, and this function will not be called.
	 * @param[in] candidateListLength
	 * The number of candidates the search should return.
	 * @param[out] candidateList
	 * Each candidate shall be populated by the implementation.  The candidates 
	 * shall appear in descending order of similarity score - i.e. most similar 
	 * entries appear first.
	 */
	virtual ReturnStatus
	identifyTemplate(
		const std::vector<uint8_t> &idTemplate,
		const uint32_t candidateListLength,
		std::vector<Candidate> &candidateList) = 0;

    /**
     * @brief REQUIRED.  This function initializes the implementation under test.  It will
     * be called by the NIST application before any call to the function
     * detectTattoo().
     *
     * @param[in] configDir
     * A read-only directory containing any developer-supplied configuration
     * parameters or run-time data.  The name of this directory is assigned by
     * NIST, not hardwired by the provider.  The names of the
     * files in this directory are hardwired in the implementation and are
     * unrestricted.
     */
    virtual ReturnStatus
    initializeDetection(
        const std::string &configDir) = 0;

    /**
     * @brief REQUIRED.  This function takes an Image as input and indicates
     * whether a tattoo on skin was detected in the image or not.
     *
     * @param[in] inputImage
     * An instance of an Image struct representing a single image
     * @param[out] tattooDetected
     * true if a tattoo on skin is detected in the image;
     * false otherwise
     * @param[out] confidence
     * A real-valued measure of tattoo detection confidence on [0,1].
     * A value of 1 indicates certainty that the image contains a tattoo,
     * and a value of 0 indicates certainty that the image does not contain a
     * tattoo.
	 * @param[out] boundingBoxes
     * For each tattoo detected in the image, the function shall create a
     * BoundingBox, populate it with the x, y, width,
     * height of the bounding box, and add it to the vector.
     */
    virtual ReturnStatus
    detectTattoo(
        const Image &inputImage,
        bool &tattooDetected,
        double &confidence,
		std::vector<BoundingBox> &boundingBoxes) = 0;

	/**
	 * @brief REQUIRED.  Factory method to return a managed pointer to the Interface 
	 * object.
	 *
	 * @details
	 * This function is implemented by the submitted library and must return
	 * a managed pointer to the Interface object.
	 *
	 * @note
	 * A possible implementation might be:
	 * return (std::make_shared<Implementation>());
	 */
	static std::shared_ptr<Interface>
	getImplementation();
};

/*
 * API versioning
 *
 * NIST code will extern the version number symbols.
 * Participant shall compile them into their core library.
 */
#ifdef NIST_EXTERN_API_VERSION
/** API major version number. */
extern uint16_t API_MAJOR_VERSION;
/** API minor version number. */
extern uint16_t API_MINOR_VERSION;
#else /* NIST_EXTERN_API_VERSION */
/** API major version number. */
uint16_t API_MAJOR_VERSION{0};
/** API minor version number. */
uint16_t API_MINOR_VERSION{1};
#endif /* NIST_EXTERN_API_VERSION */

} /* End of namespace */

#endif /* TATTE_H_ */
