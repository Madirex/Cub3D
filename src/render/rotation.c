/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:18 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 16:27:26 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"
#include <math.h>

/* Rotate right by angle (used by perform_movements) */
void	rotate_right(t_cub3d *cub, double rot_speed_dt)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(rot_speed_dt)
		- cub->dir_y * sin(rot_speed_dt);
	cub->dir_y = old_dir_x * sin(rot_speed_dt)
		+ cub->dir_y * cos(rot_speed_dt);
	old_plane_x = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(rot_speed_dt)
		- cub->plane_y * sin(rot_speed_dt);
	cub->plane_y = old_plane_x * sin(rot_speed_dt)
		+ cub->plane_y * cos(rot_speed_dt);
}

/* Rotate left by angle (used by perform_movements) */
void	rotate_left(t_cub3d *cub, double rot_speed_dt)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(-rot_speed_dt)
		- cub->dir_y * sin(-rot_speed_dt);
	cub->dir_y = old_dir_x * sin(-rot_speed_dt)
		+ cub->dir_y * cos(-rot_speed_dt);
	old_plane_x = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(-rot_speed_dt)
		- cub->plane_y * sin(-rot_speed_dt);
	cub->plane_y = old_plane_x * sin(-rot_speed_dt)
		+ cub->plane_y * cos(-rot_speed_dt);
}
