#ifndef CUB3D_H
# define CUB3D_H

#include <stdio.h>
#include "libft.h"

typedef struct s_rgb
{
	int	r;
	int	g;
	int	b;
}	t_rgb;

typedef struct s_textures
{
	char	*no;
	char	*so;
	char	*we;
	char	*ea;
	t_rgb	floor;
	t_rgb	ceiling;
}	t_textures;

typedef struct s_cub3d
{
	t_textures	textures;
	char		**map;
	int			map_width;
	int			map_height;
}	t_cub3d;

#endif
