#ifndef PARSE_MAP_H
# define PARSE_MAP_H

# include "cub3d.h"

void	assign_map(t_cub3d *cub, char *filename);
int		is_valid_map_char(char c);
int		is_map_line(const char *line);

#endif
