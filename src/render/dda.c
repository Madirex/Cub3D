#include "../../includes/cub3d_render.h"
#include <math.h>

/* DDA helpers and main DDA routine */

/* Initialize delta distances (1/rayDir) */
void init_delta_dist(t_dda_in *in, double *deltaDistX, double *deltaDistY)
{
	if (in->rayDirX == 0)
		*deltaDistX = 1e30;
	else
		*deltaDistX = fabs(1.0 / in->rayDirX);
	if (in->rayDirY == 0)
		*deltaDistY = 1e30;
	else
		*deltaDistY = fabs(1.0 / in->rayDirY);
}

/* Initialize DDA steps and initial side distances */
void init_dda_steps(t_cub3d *cub, t_dda_in *in, t_dda_out *out, double *sideDistX, double *sideDistY)
{
	double	deltaDistX;
	double	deltaDistY;

	init_delta_dist(in, &deltaDistX, &deltaDistY);
	if (in->rayDirX < 0)
	{
		out->stepX = -1;
		*sideDistX = (cub->pos_x - in->mapX) * deltaDistX;
	}
	else
	{
		out->stepX = 1;
		*sideDistX = (in->mapX + 1.0 - cub->pos_x) * deltaDistX;
	}
	if (in->rayDirY < 0)
	{
		out->stepY = -1;
		*sideDistY = (cub->pos_y - in->mapY) * deltaDistY;
	}
	else
	{
		out->stepY = 1;
		*sideDistY = (in->mapY + 1.0 - cub->pos_y) * deltaDistY;
	}
}

/* Perform one DDA step update */
void dda_step_update(t_dda_in *in, t_dda_out *out, double *sideDistX, double *sideDistY)
{
	double	invRx;
	double	invRy;

	if (*sideDistX < *sideDistY)
	{
		if (in->rayDirX == 0)
			invRx = 1e30;
		else
			invRx = 1.0 / in->rayDirX;
		*sideDistX += fabs(invRx);
		in->mapX += out->stepX;
		out->side = 0;
	}
	else
	{
		if (in->rayDirY == 0)
			invRy = 1e30;
		else
			invRy = 1.0 / in->rayDirY;
		*sideDistY += fabs(invRy);
		in->mapY += out->stepY;
		out->side = 1;
	}
}

/* Compute perpendicular wall distance for hit */
void compute_perp_wall_dist(t_dda_in *in, t_dda_out *out, double sideDistX, double sideDistY)
{
	double	inv;

	if (out->side == 0)
	{
		if (in->rayDirX == 0)
			inv = 1e30;
		else
			inv = 1.0 / in->rayDirX;
		out->perpWallDist = sideDistX - fabs(inv);
	}
	else
	{
		if (in->rayDirY == 0)
			inv = 1e30;
		else
			inv = 1.0 / in->rayDirY;
		out->perpWallDist = sideDistY - fabs(inv);
	}
}

/* Perform the DDA loop and detect wall/door hit */
int perform_dda(t_cub3d *cub, t_dda_in *in, t_dda_out *out)
{
	double	sideDistX;
	double	sideDistY;

	init_dda_steps(cub, in, out, &sideDistX, &sideDistY);
	while (1)
	{
		dda_step_update(in, out, &sideDistX, &sideDistY);
		if (in->mapY < 0 || in->mapY >= cub->map_height
			|| in->mapX < 0
			|| in->mapX >= (int)strlen(cub->map[in->mapY]))
			return (0);
		if (cub->map[in->mapY][in->mapX] == '1'
			|| (cub->is_bonus && cub->map[in->mapY][in->mapX] == 'D'))
		{
			compute_perp_wall_dist(in, out, sideDistX, sideDistY);
			return (1);
		}
	}
}