#ifndef UTILS_H
# define UTILS_H

# include "cub3d.h"

void free_textures(t_textures *textures);
void free_map(char **map, int height);
void safe_exit(t_cub3d *cub, char *line);

#endif
