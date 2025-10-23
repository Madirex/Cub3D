/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard_mouse.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:39 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 15:44:03 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"

void	safe_exit(t_cub3d *cub, char *line, int exit_code);

/* Input callbacks (keyboard & mouse) */

/* Key press handler */
int	handle_key_press(int key, t_cub3d *cub)
{
	if (key == 119)
		cub->is_moving_forward = 1;
	else if (key == 115)
		cub->is_moving_backward = 1;
	else if (key == 97 || key == 65361)
		cub->is_rotating_left = 1;
	else if (key == 100 || key == 65363)
		cub->is_rotating_right = 1;
	else if (key == 65307)
		safe_exit(cub, NULL, 0);
	else if (key == 49 || key == 32 || key == 65349)
		handle_door_action(cub);
	return (0);
}

/* Key release handler */
int	handle_key_release(int key, t_cub3d *cub)
{
	if (key == 119)
		cub->is_moving_forward = 0;
	else if (key == 115)
		cub->is_moving_backward = 0;
	else if (key == 97 || key == 65361)
		cub->is_rotating_left = 0;
	else if (key == 100 || key == 65363)
		cub->is_rotating_right = 0;
	return (0);
}

/* Mouse move handler: rotate player based on delta X */
int	handle_mouse_move(int x, int y, t_cub3d *cub)
{
	int		delta_x;
	double	rotation_angle;

	(void)y;
	delta_x = x - cub->last_mouse_x;
	if (delta_x == 0)
		return (0);
	rotation_angle = delta_x * MOUSE_SENSITIVITY;
	rotate_player(cub, rotation_angle);
	cub->last_mouse_x = x;
	return (0);
}
