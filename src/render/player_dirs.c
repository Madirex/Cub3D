/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_dirs.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:22 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 14:32:56 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"
#include <math.h>

/* Player direction setters */

/* Facing North */
void	set_dir_n(t_cub3d *cub)
{
	cub->dir_x = 0;
	cub->dir_y = -1;
	cub->plane_x = 0.66;
	cub->plane_y = 0;
}

/* Facing South */
void	set_dir_s(t_cub3d *cub)
{
	cub->dir_x = 0;
	cub->dir_y = 1;
	cub->plane_x = -0.66;
	cub->plane_y = 0;
}

/* Facing East */
void	set_dir_e(t_cub3d *cub)
{
	cub->dir_x = 1;
	cub->dir_y = 0;
	cub->plane_x = 0;
	cub->plane_y = 0.66;
}

/* Facing West */
void	set_dir_w(t_cub3d *cub)
{
	cub->dir_x = -1;
	cub->dir_y = 0;
	cub->plane_x = 0;
	cub->plane_y = -0.66;
}

/* Set player direction from a map char */
void	set_player_dir(t_cub3d *cub, char dir)
{
	if (dir == 'N')
		set_dir_n(cub);
	else if (dir == 'S')
		set_dir_s(cub);
	else if (dir == 'E')
		set_dir_e(cub);
	else if (dir == 'W')
		set_dir_w(cub);
}
