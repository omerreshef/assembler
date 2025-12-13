/**
 * Scanning parsed lines to determine the sizes of instruction and data images for object file header.
 * @param parsed_lines The array of parsed lines from the first pass.
 * @param instruction_image_size Pointer to store the calculated size of the instruction image.
 * @param data_image_size Pointer to store the calculated size of the data image.
 * 
 * On success, fills instruction_image_size and data_image_size with the calculated sizes and returns SUCCESS.
 */
RC_t files_creator__get_obj_file_header(parsed_lines_t *parsed_lines, int *instruction_image_size, int *data_image_size);