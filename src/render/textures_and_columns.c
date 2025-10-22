/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   textures_and_columns.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:03 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 14:35:52 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"
#include <string.h>

/* Texture selection and basic column drawing helpers */

/* Choose the texture buffer for a hit cell
 (doors / adjacent open door handling) */
int	*select_texture_buffer(t_cub3d *cub, t_tex_query *q)
{
	int		cell_hit;
	int		neighbor_x;
	int		neighbor_y;
	int		is_door_adjacent;

	cell_hit = cub->map[q->map_y][q->map_x];
	if (cub->is_bonus && cell_hit == 'D' && cub->door_textures
		&& cub->door_textures[cub->door_anim_frame] != NULL)
		return (cub->door_textures[cub->door_anim_frame]);
	neighbor_x = q->map_x;
	neighbor_y = q->map_y;
	if (q->side == 0)
		neighbor_x = q->map_x - q->step_x;
	else
		neighbor_y = q->map_y - q->step_y;
	is_door_adjacent = 0;
	if (cub->is_bonus && neighbor_y >= 0 && neighbor_y < cub->map_height
		&& neighbor_x >= 0 && neighbor_x < (int)strlen(cub->map[neighbor_y])
		&& cub->map[neighbor_y][neighbor_x] == 'O'
		&& cub->door_textures && cub->door_textures[1] != NULL)
		is_door_adjacent = 1;
	if (is_door_adjacent)
		return (cub->door_textures[1]);
	return (cub->wall_textures[get_wall_texture(q->side, 0, 0)]);
}

/* Draw ceiling column part */
void	draw_column_ceiling(t_img *img, t_col_geom *g, int ceiling)
{
	int				y;
	unsigned int	*pixel;
	unsigned char	*addr;

	y = 0;
	while (y < g->draw_start)
	{
		addr = (unsigned char *)img->data;
		addr += y * img->size_line;
		addr += g->x * img->bytes_per_pixel;
		pixel = (unsigned int *)addr;
		*pixel = (unsigned int)ceiling;
		y++;
	}
}

/* Draw floor column part */
void	draw_column_floor(t_img *img, t_col_geom *g, int floor)
{
	int				y;
	unsigned int	*pixel;
	unsigned char	*addr;

	y = g->draw_end + 1;
	while (y < HEIGHT)
	{
		addr = (unsigned char *)img->data;
		addr += y * img->size_line;
		addr += g->x * img->bytes_per_pixel;
		pixel = (unsigned int *)addr;
		*pixel = (unsigned int)floor;
		y++;
	}
}

/* Draw both ceiling and floor for a given column */
void	draw_column_floor_ceiling(t_img *img,
	t_col_geom *g, t_render_colors *cols)
{
	draw_column_ceiling(img, g, cols->ceiling);
	draw_column_floor(img, g, cols->floor);
}

/* Draw a textured pixel with shading based on side */
void	draw_textured_pixel(t_img *img, t_col_geom *g, int y, int color)
{
	unsigned int	*pixel;
	unsigned char	*addr;

	if (g->side == 1)
		color = (color >> 1) & 0x7F7F7F;
	addr = (unsigned char *)img->data;
	addr += y * img->size_line;
	addr += g->x * img->bytes_per_pixel;
	pixel = (unsigned int *)addr;
	*pixel = (unsigned int)color;
}
