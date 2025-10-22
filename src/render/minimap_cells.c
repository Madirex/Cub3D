/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_cells.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:35 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 14:38:51 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"
#include <string.h>

/* Minimap helpers: cell color and cell drawing */

/* Determine cell color based on map content */
int	minimap_cell_color(t_cub3d *cub, int map_y, int map_x)
{
	char	cell;

	if (map_y < 0 || map_y >= cub->map_height)
		return (COLOR_EMPTY);
	if (map_x < 0 || cub->map[map_y] == NULL)
		return (COLOR_EMPTY);
	if (map_x >= (int)strlen(cub->map[map_y]))
		return (COLOR_EMPTY);
	cell = cub->map[map_y][map_x];
	if (cell == '1' || cell == 'D')
		return (COLOR_WALL);
	if (IS_FLOOR(cell))
		return (COLOR_FLOOR);
	return (COLOR_EMPTY);
}

/* Fill entire minimap cell area with a color */
void	minimap_fill_cell(t_img *img, int base_x, int base_y, int color)
{
	int	px;
	int	py;

	py = 0;
	while (py < MINIMAP_SCALE)
	{
		px = 0;
		while (px < MINIMAP_SCALE)
		{
			draw_pixel(img, base_x + px, base_y + py, color);
			px++;
		}
		py++;
	}
}

/* Draw top and left border of a minimap cell */
void	minimap_draw_border(t_img *img, int draw_x, int draw_y)
{
	int	i;

	i = 0;
	while (i < MINIMAP_SCALE)
	{
		draw_pixel(img, draw_x + i, draw_y, 0x000000);
		draw_pixel(img, draw_x, draw_y + i, 0x000000);
		i++;
	}
}

/* Draw a whole cell (fill + border) */
void	minimap_draw_cell(t_img *img, int draw_x, int draw_y, int color)
{
	minimap_fill_cell(img, draw_x, draw_y, color);
	minimap_draw_border(img, draw_x, draw_y);
}

/* Draw one row of minimap cells relative to player.
** Now accepts a single context pointer to respect parameter limit.
*/
void	minimap_draw_row(t_minimap_ctx *ctx, int i)
{
	t_minimap_row_ctx	lctx;

	lctx.j = -MINIMAP_VIEW_RADIUS;
	while (lctx.j <= MINIMAP_VIEW_RADIUS)
	{
		lctx.map_cell_x = (int)ctx->cub->pos_x + lctx.j;
		lctx.map_cell_y = (int)ctx->cub->pos_y + i;
		lctx.cell_draw_x = ctx->start_x
			+ (lctx.j + MINIMAP_VIEW_RADIUS) * MINIMAP_SCALE;
		lctx.cell_draw_y = ctx->start_y
			+ (i + MINIMAP_VIEW_RADIUS) * MINIMAP_SCALE;
		lctx.color = minimap_cell_color(ctx->cub, lctx.map_cell_y,
				lctx.map_cell_x);
		minimap_draw_cell(ctx->img, lctx.cell_draw_x, lctx.cell_draw_y,
			lctx.color);
		lctx.j++;
	}
}
