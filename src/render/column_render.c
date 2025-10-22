/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   column_render.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:59 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 14:36:51 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"
#include <math.h>

/* Column rasterization and per-column pipeline */

/* Draw textured column using texture buffer */
void	draw_column_textured(t_cub3d *cub, t_img *img,
	t_col_geom *g, int *tex_buf)
{
	int				y;
	double			step;
	double			tex_pos;
	int				tex_y;
	int				color;

	if (g->line_height <= 0 || tex_buf == NULL)
		return ;
	step = 1.0 * cub->tex_height / g->line_height;
	tex_pos = (g->draw_start - HEIGHT / 2 + g->line_height / 2) * step;
	y = g->draw_start;
	while (y <= g->draw_end)
	{
		tex_y = (int)tex_pos & (cub->tex_height - 1);
		tex_pos += step;
		color = tex_buf[cub->tex_width * tex_y + g->tex_x];
		draw_textured_pixel(img, g, y, color);
		y++;
	}
}

/* Compute geometry parameters for a column */
void	compute_column_params(t_cub3d *cub, t_col_calc_in *in, t_col_geom *g)
{
	g->line_height = (int)(HEIGHT / in->perp_wall_dist);
	g->draw_start = -g->line_height / 2 + HEIGHT / 2;
	if (g->draw_start < 0)
		g->draw_start = 0;
	g->draw_end = g->line_height / 2 + HEIGHT / 2;
	if (g->draw_end >= HEIGHT)
		g->draw_end = HEIGHT - 1;
	if (in->side == 0)
		g->wall_x = cub->pos_y + in->perp_wall_dist * in->ray_dir_y;
	else
		g->wall_x = cub->pos_x + in->perp_wall_dist * in->ray_dir_x;
	g->wall_x -= floor(g->wall_x);
	g->tex_x = (int)(g->wall_x * (double)cub->tex_width);
	g->side = in->side;
}

/* Adjust tex_x for flipping depending on side and ray direction */
void	render_column_adjust_texx(t_col_geom *g,
	t_dda_in *dda_in, int tex_width)
{
	if (g->side == 0 && dda_in->ray_dir_x > 0)
		g->tex_x = tex_width - g->tex_x - 1;
	if (g->side == 1 && dda_in->ray_dir_y < 0)
		g->tex_x = tex_width - g->tex_x - 1;
}

/* Render one column (full pipeline) */
void	render_column(t_cub3d *cub, t_img *img, int x, t_render_colors *cols)
{
	t_render_ctx	ctx;

	ctx.dda_in.map_x = (int)cub->pos_x;
	ctx.dda_in.map_y = (int)cub->pos_y;
	ctx.dda_in.ray_dir_x = cub->dir_x + cub->plane_x
		* (2 * x / (double)WIDTH - 1);
	ctx.dda_in.ray_dir_y = cub->dir_y + cub->plane_y
		* (2 * x / (double)WIDTH - 1);
	if (!perform_dda(cub, &ctx.dda_in, &ctx.dda_out))
		return ;
	ctx.calc_in.perp_wall_dist = ctx.dda_out.perp_wall_dist;
	ctx.calc_in.side = ctx.dda_out.side;
	ctx.calc_in.ray_dir_x = ctx.dda_in.ray_dir_x;
	ctx.calc_in.ray_dir_y = ctx.dda_in.ray_dir_y;
	ctx.g.x = x;
	compute_column_params(cub, &ctx.calc_in, &ctx.g);
	render_column_adjust_texx(&ctx.g, &ctx.dda_in, cub->tex_width);
	ctx.q.side = ctx.g.side;
	ctx.q.map_x = ctx.dda_in.map_x;
	ctx.q.map_y = ctx.dda_in.map_y;
	ctx.q.step_x = ctx.dda_out.step_x;
	ctx.q.step_y = ctx.dda_out.step_y;
	ctx.tex_buf = select_texture_buffer(cub, &ctx.q);
	draw_column_floor_ceiling(img, &ctx.g, cols);
	draw_column_textured(cub, img, &ctx.g, ctx.tex_buf);
}

/* Render the whole screen (raycast) */
void	raycast_render(t_cub3d *cub, t_img *img)
{
	int				x;
	t_render_colors	cols;

	img->bytes_per_pixel = img->bpp / 8;
	if (cub->textures.ceiling.r != -1)
		cols.ceiling = rgb_to_int(cub->textures.ceiling.r,
				cub->textures.ceiling.g,
				cub->textures.ceiling.b);
	else
		cols.ceiling = 0x202020;
	if (cub->textures.floor.r != -1)
		cols.floor = rgb_to_int(cub->textures.floor.r,
				cub->textures.floor.g,
				cub->textures.floor.b);
	else
		cols.floor = 0x707070;
	x = 0;
	while (x < WIDTH)
	{
		render_column(cub, img, x, &cols);
		x++;
	}
}
