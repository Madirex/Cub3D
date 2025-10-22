#include "../../includes/cub3d_render.h"
#include <math.h>

/*
** DDA initialization helpers: init_delta_dist + helpers to compute sx/sy and
** the public init_dda_steps function.
*/

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

/*
** Helpers to compute initial side distances and steps.
** Kept static because they are internal implementation details.
*/
static double compute_init_sx(t_cub3d *cub, t_dda_in *in, double deltaDistX, int *out_stepX)
{
	if (in->rayDirX < 0)
	{
		*out_stepX = -1;
		return ((cub->pos_x - in->mapX) * deltaDistX);
	}
	*out_stepX = 1;
	return ((in->mapX + 1.0 - cub->pos_x) * deltaDistX);
}

static double compute_init_sy(t_cub3d *cub, t_dda_in *in, double deltaDistY, int *out_stepY)
{
	if (in->rayDirY < 0)
	{
		*out_stepY = -1;
		return ((cub->pos_y - in->mapY) * deltaDistY);
	}
	*out_stepY = 1;
	return ((in->mapY + 1.0 - cub->pos_y) * deltaDistY);
}

/*
** Initialize DDA steps and initial side distances.
** Returns a t_dda_side containing the initial side distances.
*/
t_dda_side init_dda_steps(t_cub3d *cub, t_dda_in *in, t_dda_out *out)
{
	t_dda_side	s;
	double		deltaDistX;
	double		deltaDistY;

	init_delta_dist(in, &deltaDistX, &deltaDistY);
	s.sx = compute_init_sx(cub, in, deltaDistX, &out->stepX);
	s.sy = compute_init_sy(cub, in, deltaDistY, &out->stepY);
	return (s);
}