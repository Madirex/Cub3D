/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:26 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 15:38:57 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:26 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 14:33:13 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d_render.h"
#include <math.h>

/* Movement and rotation helpers and orchestrator */

/* Move forward with collision checks */
void	move_forward(t_cub3d *cub, double move_speed_dt)
{
	if (is_floor(cub->map[(int)(cub->pos_y)]
		[(int)(cub->pos_x + cub->dir_x * move_speed_dt)]))
		cub->pos_x += cub->dir_x * move_speed_dt;
	if (is_floor(cub->map[(int)(cub->pos_y + cub->dir_y * move_speed_dt)]
		[(int)(cub->pos_x)]))
		cub->pos_y += cub->dir_y * move_speed_dt;
}

/* Move backward with collision checks */
void	move_backward(t_cub3d *cub, double move_speed_dt)
{
	if (is_floor(cub->map[(int)(cub->pos_y)]
		[(int)(cub->pos_x - cub->dir_x * move_speed_dt)]))
		cub->pos_x -= cub->dir_x * move_speed_dt;
	if (is_floor(cub->map[(int)(cub->pos_y - cub->dir_y * move_speed_dt)]
		[(int)(cub->pos_x)]))
		cub->pos_y -= cub->dir_y * move_speed_dt;
}

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

/* Called every frame to apply movement/rotation flags */
void	perform_movements(t_cub3d *cub)
{
	double	move_speed_dt;
	double	rot_speed_dt;

	move_speed_dt = MOVE_SPEED * cub->time_frame * 60.0;
	rot_speed_dt = ROT_SPEED * cub->time_frame * 60.0;
	if (cub->is_moving_forward)
		move_forward(cub, move_speed_dt);
	if (cub->is_moving_backward)
		move_backward(cub, move_speed_dt);
	if (cub->is_rotating_right)
		rotate_right(cub, rot_speed_dt);
	if (cub->is_rotating_left)
		rotate_left(cub, rot_speed_dt);
}
