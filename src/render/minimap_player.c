#include "../includes/cub3d_render.h"

/* Minimap composition and player drawing */

/* Draw minimap rows around player using the context */
void minimap_draw_cells(t_minimap_ctx *ctx)
{
	int	i;

	i = -MINIMAP_VIEW_RADIUS;
	while (i <= MINIMAP_VIEW_RADIUS)
	{
		minimap_draw_row(ctx, i);
		i++;
	}
}

/* Draw small square representing the player on minimap */
void minimap_draw_player_square(t_img *img, int player_pixel_x, int player_pixel_y)
{
	int	player_size;
	int	y;
	int	x;

	player_size = MINIMAP_SCALE / 4;
	y = player_pixel_y - player_size / 2;
	while (y < player_pixel_y + player_size / 2)
	{
		x = player_pixel_x - player_size / 2;
		while (x < player_pixel_x + player_size / 2)
		{
			draw_pixel(img, x, y, COLOR_PLAYER);
			x++;
		}
		y++;
	}
}

/* Draw simple direction indicator on minimap */
void minimap_draw_player_dir(t_img *img, int player_pixel_x, int player_pixel_y, t_vec2 *dir)
{
	int	dir_len;
	int	dir_end_x;
	int	dir_end_y;

	dir_len = MINIMAP_SCALE / 2;
	dir_end_x = player_pixel_x + (int)(dir->x * dir_len);
	dir_end_y = player_pixel_y + (int)(dir->y * dir_len);
	draw_pixel(img, dir_end_x, dir_end_y, COLOR_PLAYER);
	draw_pixel(img, dir_end_x + 1, dir_end_y, COLOR_PLAYER);
	draw_pixel(img, dir_end_x, dir_end_y + 1, COLOR_PLAYER);
	draw_pixel(img, dir_end_x - 1, dir_end_y, COLOR_PLAYER);
	draw_pixel(img, dir_end_x, dir_end_y - 1, COLOR_PLAYER);
}

/* Compute the player pixel coords inside the minimap area.
** Returns a t_point to avoid out-parameters.
*/
t_point minimap_compute_player_coords(t_minimap_ctx *ctx)
{
	t_point p;
	int	player_center_x;
	int	player_center_y;
	double	relative_x;
	double	relative_y;

	player_center_x = ctx->start_x + MINIMAP_VIEW_RADIUS * MINIMAP_SCALE
		+ MINIMAP_SCALE / 2;
	player_center_y = ctx->start_y + MINIMAP_VIEW_RADIUS * MINIMAP_SCALE
		+ MINIMAP_SCALE / 2;
	relative_x = ctx->cub->pos_x - (int)ctx->cub->pos_x;
	relative_y = ctx->cub->pos_y - (int)ctx->cub->pos_y;
	p.x = player_center_x + (int)((relative_x - 0.5) * MINIMAP_SCALE);
	p.y = player_center_y + (int)((relative_y - 0.5) * MINIMAP_SCALE);
	return (p);
}

/* Public entrypoint to draw the minimap */
void draw_minimap(t_cub3d *cub, t_img *img)
{
	t_minimap_ctx ctx;
	t_point p;
	t_vec2 dir;

	ctx.cub = cub;
	ctx.img = img;
	ctx.start_x = MINIMAP_MARGIN;
	ctx.start_y = MINIMAP_MARGIN;
	minimap_draw_cells(&ctx);
	p = minimap_compute_player_coords(&ctx);
	minimap_draw_player_square(img, p.x, p.y);
	dir.x = cub->dir_x;
	dir.y = cub->dir_y;
	minimap_draw_player_dir(img, p.x, p.y, &dir);
}