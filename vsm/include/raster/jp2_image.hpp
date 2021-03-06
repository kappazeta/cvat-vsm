// Tiled loading of a JP2 image
//
// Copyright 2020 KappaZeta Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "raster/raster_image.hpp"

#include <filesystem>


class JP2_Image: public RasterImage {
	public:
		JP2_Image();
		~JP2_Image();

		/**
		 * Load the header of a JP2 file.
		 * @param[in] path Path to the JP2 file.
		 * @return True on success, False otherwise.
		 */
		bool load_header(const std::filesystem::path &path);

		/**
		 * Load a subset of a JP2 file.
		 * @param[in] path Path to the JP2 file.
		 * @param[in] da_x0 Left side of the decode area, in pixels.
		 * @param[in] da_y0 Top side of the decode area, in pixels.
		 * @param[in] da_x1 Right side of the decode area, in pixels.
		 * @param[in] da_y1 Bottom side of the decode area, in pixels.
		 * @return True on success, False otherwise.
		 */
		bool load_subset(const std::filesystem::path &path, int da_x0, int da_y0, int da_x1, int da_y1);

		/**
		 * Load the whole JP2 file in RAM.
		 * @param[in] path Path to the JP2 file.
		 * @return True on success, False otherwise.
		 */
		bool load_whole(const std::filesystem::path &path);

		/**
		 * Subset the whole JP2 file.
		 * @param[in] da_x0 Left side of the decode area, in pixels.
		 * @param[in] da_y0 Top side of the decode area, in pixels.
		 * @param[in] da_x1 Right side of the decode area, in pixels.
		 * @param[in] da_y1 Bottom side of the decode area, in pixels.
		 * @return True on success, False otherwise.
		 */
		bool subset_whole(int da_x0, int da_y0, int da_x1, int da_y1);

		static void error_callback(const char *msg, void *client_data);

		static void warning_callback(const char *msg, void *client_data);

		static void info_callback(const char *msg, void *client_data);

	private:
		//! The whole decoded image.
		Magick::Image *whole_image;
};

