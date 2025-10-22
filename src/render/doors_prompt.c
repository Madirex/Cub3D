/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doors_prompt.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:43 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 14:38:39 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mlx.h>
#include "../../includes/cub3d_render.h"

/* Door prompt helpers and rendering */

/* Check near/far target logic used for drawing prompt.
** Returns a t_target with .ok == 1 when a valid target was found.
*/
t_target	check_near_far_target(t_cub3d *cub, double near_d, double far_d)
{
	t_target	t;

	t.ok = 0;
	if (!get_target_cell_coords(cub, near_d, &t.x, &t.y))
		return (t);
	if ((int)cub->pos_x == t.x && (int)cub->pos_y == t.y)
	{
		if (!get_target_cell_coords(cub, far_d, &t.x, &t.y))
			return (t);
	}
	t.ok = 1;
	return (t);
}

/* Return prompt string based on door cell state */
const char	*select_door_message(char cell)
{
	if (cell == 'D')
		return ("Press SPACE to open");
	return ("Press SPACE to close");
}

/* Draw door prompt on screen if applicable */
void	draw_door_prompt(t_cub3d *cub)
{
	t_target	tgt;
	const char	*message;
	char		target_cell;

	tgt = check_near_far_target(cub, 0.75, 1.5);
	if (!tgt.ok)
		return ;
	target_cell = cub->map[tgt.y][tgt.x];
	if (cub->is_bonus && (target_cell == 'D' || target_cell == 'O'))
	{
		message = select_door_message(target_cell);
		mlx_string_put(cub->mlx, cub->win,
			(WIDTH / 2) - 100,
			HEIGHT - 50,
			0xFFFFFF,
			(char *)message);
	}
}
