/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dda_loop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:51 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 16:27:05 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"
#include <math.h>
#include <string.h>

/* Perform one DDA step update */
void	dda_step_update(t_dda_in *in, t_dda_out *out,
	double *side_dist_x, double *side_dist_y)
{
	double	inv_rx;
	double	inv_ry;

	if (*side_dist_x < *side_dist_y)
	{
		if (in->ray_dir_x == 0)
			inv_rx = 1e30;
		else
			inv_rx = 1.0 / in->ray_dir_x;
		*side_dist_x += fabs(inv_rx);
		in->map_x += out->step_x;
		out->side = 0;
	}
	else
	{
		if (in->ray_dir_y == 0)
			inv_ry = 1e30;
		else
			inv_ry = 1.0 / in->ray_dir_y;
		*side_dist_y += fabs(inv_ry);
		in->map_y += out->step_y;
		out->side = 1;
	}
}

/* Compute perpendicular wall distance for hit */
void	compute_perp_wall_dist(t_dda_in *in, t_dda_out *out,
	double side_dist_x, double side_dist_y)
{
	double	inv;

	if (out->side == 0)
	{
		if (in->ray_dir_x == 0)
			inv = 1e30;
		else
			inv = 1.0 / in->ray_dir_x;
		out->perp_wall_dist = side_dist_x - fabs(inv);
	}
	else
	{
		if (in->ray_dir_y == 0)
			inv = 1e30;
		else
			inv = 1.0 / in->ray_dir_y;
		out->perp_wall_dist = side_dist_y - fabs(inv);
	}
}

/* Perform the DDA loop and detect wall/door hit */
int	perform_dda(t_cub3d *cub, t_dda_in *in, t_dda_out *out)
{
	double		side_dist_x;
	double		side_dist_y;
	t_dda_side	sides;

	sides = init_dda_steps(cub, in, out);
	side_dist_x = sides.sx;
	side_dist_y = sides.sy;
	while (1)
	{
		dda_step_update(in, out, &side_dist_x, &side_dist_y);
		if (in->map_y < 0 || in->map_y >= cub->map_height
			|| in->map_x < 0
			|| in->map_x >= (int)ft_strlen(cub->map[in->map_y]))
			return (0);
		if (cub->map[in->map_y][in->map_x] == '1'
			|| (cub->is_bonus && cub->map[in->map_y][in->map_x] == 'D'))
		{
			compute_perp_wall_dist(in, out, side_dist_x, side_dist_y);
			return (1);
		}
	}
}
