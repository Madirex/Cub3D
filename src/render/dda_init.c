/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dda_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:55 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 14:37:20 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"
#include <math.h>

/* Initialize delta distances (1/rayDir) */
void	init_delta_dist(t_dda_in *in,
	double *delta_dist_x, double *delta_dist_y)
{
	if (in->ray_dir_x == 0)
		*delta_dist_x = 1e30;
	else
		*delta_dist_x = fabs(1.0 / in->ray_dir_x);
	if (in->ray_dir_y == 0)
		*delta_dist_y = 1e30;
	else
		*delta_dist_y = fabs(1.0 / in->ray_dir_y);
}

/*
** Helpers to compute initial side distances and steps.
** Kept static (internal) to avoid polluting API.
*/
static double	compute_init_sx(t_cub3d *cub, t_dda_in *in,
	double delta_dist_x, int *out_step_x)
{
	if (in->ray_dir_x < 0)
	{
		*out_step_x = -1;
		return ((cub->pos_x - in->map_x) * delta_dist_x);
	}
	*out_step_x = 1;
	return ((in->map_x + 1.0 - cub->pos_x) * delta_dist_x);
}

static double	compute_init_sy(t_cub3d *cub, t_dda_in *in,
	double delta_dist_y, int *out_step_y)
{
	if (in->ray_dir_y < 0)
	{
		*out_step_y = -1;
		return ((cub->pos_y - in->map_y) * delta_dist_y);
	}
	*out_step_y = 1;
	return ((in->map_y + 1.0 - cub->pos_y) * delta_dist_y);
}

/*
** Initialize DDA steps and initial side distances.
** Returns a t_dda_side containing the initial side distances.
*/
t_dda_side	init_dda_steps(t_cub3d *cub, t_dda_in *in, t_dda_out *out)
{
	t_dda_side	s;
	double		delta_dist_x;
	double		delta_dist_y;

	init_delta_dist(in, &delta_dist_x, &delta_dist_y);
	s.sx = compute_init_sx(cub, in, delta_dist_x, &out->step_x);
	s.sy = compute_init_sy(cub, in, delta_dist_y, &out->step_y);
	return (s);
}
