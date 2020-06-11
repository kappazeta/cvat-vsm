#include "raster/esa_s2.hpp"
#include "raster/esa_s2_scl_jp2.hpp"

#include "util/text.hpp"


ESA_S2_Image::ESA_S2_Image(): tile_size(256), scl_value_map(nullptr) {}
ESA_S2_Image::~ESA_S2_Image() {}

void ESA_S2_Image::set_tile_size(int tile_size) {
	tile_size = tile_size;
}

void ESA_S2_Image::set_scl_class_map(unsigned char *class_map) {
	scl_value_map = class_map;
}

bool ESA_S2_Image::process(const std::filesystem::path &path_dir_in, const std::filesystem::path &path_dir_out, ESA_S2_Image_Operator &op) {
	for (const auto &granule_entry: std::filesystem::directory_iterator(path_dir_in.string() + "/GRANULE/")) {
		for (const auto &r10m_entry: std::filesystem::directory_iterator(granule_entry.path().string() + "/IMG_DATA/R10m/")) {
			if (endswith(r10m_entry.path().string(), "_TCI_10m.jp2")) {
				if (split_tci_jp2(r10m_entry.path(), path_dir_out, op))
					std::cout << r10m_entry.path() << std::endl;
			}
		}
		for (const auto &r20m_entry: std::filesystem::directory_iterator(granule_entry.path().string() + "/IMG_DATA/R20m/")) {
			if (endswith(r20m_entry.path().string(), "_SCL_20m.jp2")) {
				split_scl_jp2(r20m_entry.path(), path_dir_out, op);
			}
		}
	}
}

bool ESA_S2_Image::split_scl_jp2(const std::filesystem::path &path_in, const std::filesystem::path &path_dir_out, ESA_S2_Image_Operator &op) {
	ESA_S2_SCL_JP2_Image img_src;
	bool retval = true;

	// Get image dimensions.
	retval &= img_src.load_header(path_in);

	int w = img_src.w;
	int h = img_src.h;
	
	std::cout << img_src << std::endl;

	// Subset the image, and store the subsets in a dedicated directory.
	for (int y=0; y<h; y+=tile_size) {
		for (int x=0; x<w; x+=tile_size) {
			std::cout << "Tile " << x << ", " << y << std::endl;
			img_src.load_subset(path_in, x, y, x + tile_size, y + tile_size);

			// Remap pixel values.
			if (scl_value_map != nullptr)
				img_src.remap_values(scl_value_map);

			std::ostringstream ss_path_out;
			ss_path_out << path_dir_out.string() << "/tile_" << x << "_" << y << "/";

			std::filesystem::create_directories(ss_path_out.str());

			ss_path_out << path_in.stem().string() << "_" << x << "_" << y << ".png";

			img_src.save(ss_path_out.str());
			// Potential post-processing of the file.
			if (!op(ss_path_out.str(), ESA_S2_Image_Operator::DT_SCL))
				return false;
		}
	}

	return true;
}

bool ESA_S2_Image::split_tci_jp2(const std::filesystem::path &path_in, const std::filesystem::path &path_dir_out, ESA_S2_Image_Operator &op) {
	ESA_S2_TCI_JP2_Image img_src;
	bool retval = true;

	// Get image dimensions.
	retval &= img_src.load_header(path_in);

	int w = img_src.w;
	int h = img_src.h;
	
	std::cout << img_src << std::endl;

	// Subset the image, and store the subsets in a dedicated directory.
	for (int y=0; y<h; y+=2*tile_size) {
		for (int x=0; x<w; x+=2*tile_size) {
			std::cout << "Tile " << x << ", " << y << std::endl;
			img_src.load_subset(path_in, x, y, x + 2 * tile_size, y + 2 * tile_size);

			std::ostringstream ss_path_out;
			ss_path_out << path_dir_out.string() << "/tile_" << x / 2 << "_" << y / 2 << "/";
			
			std::filesystem::create_directories(ss_path_out.str());

			ss_path_out << path_in.stem().string() << "_" << x << "_" << y << ".png";

			img_src.save(ss_path_out.str());
			// Potential post-processing of the file.
			if (!op(ss_path_out.str(), ESA_S2_Image_Operator::DT_TCI))
				return false;
		}
	}

	return true;
}
