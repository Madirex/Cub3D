#include <stdlib.h>
#include "../../includes/cub3d.h"

void free_textures(t_textures *textures)
{
	if (textures->no)
		free(textures->no);
	if (textures->so)
		free(textures->so);
	if (textures->we)
		free(textures->we);
	if (textures->ea)
		free(textures->ea);
}

void safe_exit(t_cub3d *cub, char *line)
{
	if (cub)
		free_textures(&cub->textures);
	if (line)
		free(line);
	exit(1);
}
