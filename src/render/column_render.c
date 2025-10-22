#include "../../includes/cub3d_render.h"
#include <math.h>

/* Column rasterization and per-column pipeline */

/* Draw textured column using texture buffer */
void draw_column_textured(t_cub3d *cub, t_img *img, t_col_geom *g, int *tex_buf)
{
	int				y;
	double			step;
	double			texPos;
	int				texY;
	int				color;

	if (g->lineHeight <= 0 || tex_buf == NULL)
		return ;
	step = 1.0 * cub->tex_height / g->lineHeight;
	texPos = (g->drawStart - HEIGHT / 2 + g->lineHeight / 2) * step;
	y = g->drawStart;
	while (y <= g->drawEnd)
	{
		texY = (int)texPos & (cub->tex_height - 1);
		texPos += step;
		color = tex_buf[cub->tex_width * texY + g->texX];
		draw_textured_pixel(img, g, y, color);
		y++;
	}
}

/* Compute geometry parameters for a column */
void compute_column_params(t_cub3d *cub, t_col_calc_in *in, t_col_geom *g)
{
	g->lineHeight = (int)(HEIGHT / in->perpWallDist);
	g->drawStart = -g->lineHeight / 2 + HEIGHT / 2;
	if (g->drawStart < 0)
		g->drawStart = 0;
	g->drawEnd = g->lineHeight / 2 + HEIGHT / 2;
	if (g->drawEnd >= HEIGHT)
		g->drawEnd = HEIGHT - 1;
	if (in->side == 0)
		g->wallX = cub->pos_y + in->perpWallDist * in->rayDirY;
	else
		g->wallX = cub->pos_x + in->perpWallDist * in->rayDirX;
	g->wallX -= floor(g->wallX);
	g->texX = (int)(g->wallX * (double)cub->tex_width);
	g->side = in->side;
}

/* Adjust texX for flipping depending on side and ray direction */
void render_column_adjust_texx(t_col_geom *g, t_dda_in *dda_in, int tex_width)
{
	if (g->side == 0 && dda_in->rayDirX > 0)
		g->texX = tex_width - g->texX - 1;
	if (g->side == 1 && dda_in->rayDirY < 0)
		g->texX = tex_width - g->texX - 1;
}

/* Render one column (full pipeline) */
void render_column(t_cub3d *cub, t_img *img, int x, t_render_colors *cols)
{
	t_render_ctx	ctx;

	ctx.dda_in.mapX = (int)cub->pos_x;
	ctx.dda_in.mapY = (int)cub->pos_y;
	ctx.dda_in.rayDirX = cub->dir_x + cub->plane_x
		* (2 * x / (double)WIDTH - 1);
	ctx.dda_in.rayDirY = cub->dir_y + cub->plane_y
		* (2 * x / (double)WIDTH - 1);
	if (!perform_dda(cub, &ctx.dda_in, &ctx.dda_out))
		return ;
	ctx.calc_in.perpWallDist = ctx.dda_out.perpWallDist;
	ctx.calc_in.side = ctx.dda_out.side;
	ctx.calc_in.rayDirX = ctx.dda_in.rayDirX;
	ctx.calc_in.rayDirY = ctx.dda_in.rayDirY;
	ctx.g.x = x;
	compute_column_params(cub, &ctx.calc_in, &ctx.g);
	render_column_adjust_texx(&ctx.g, &ctx.dda_in, cub->tex_width);
	ctx.q.side = ctx.g.side;
	ctx.q.mapX = ctx.dda_in.mapX;
	ctx.q.mapY = ctx.dda_in.mapY;
	ctx.q.stepX = ctx.dda_out.stepX;
	ctx.q.stepY = ctx.dda_out.stepY;
	ctx.tex_buf = select_texture_buffer(cub, &ctx.q);
	draw_column_floor_ceiling(img, &ctx.g, cols);
	draw_column_textured(cub, img, &ctx.g, ctx.tex_buf);
}

/* Render the whole screen (raycast) */
void raycast_render(t_cub3d *cub, t_img *img)
{
	int				x;
	t_render_colors	cols;

	img->bytes_per_pixel = img->bpp / 8;
	/* ceiling */
	if (cub->textures.ceiling.r != -1)
		cols.ceiling = rgb_to_int(cub->textures.ceiling.r,
				cub->textures.ceiling.g,
				cub->textures.ceiling.b);
	else
		cols.ceiling = 0x202020;
	/* floor */
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