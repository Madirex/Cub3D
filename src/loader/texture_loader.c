/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture_loader.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:48:38 by skyce11           #+#    #+#             */
/*   Updated: 2025/10/22 15:09:44 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d_render.h"
#include <stdlib.h>

/*
** Public: load wall and door textures and set cub->tex_width/tex_height.
*/
void	load_wall_textures(t_cub3d *cub, void *mlx)
{
	int	w;
	int	h;

	w = 0;
	h = 0;
	load_walls(cub, mlx, &w, &h);
	cub->tex_width = w;
	cub->tex_height = h;
	load_doors(cub, mlx, &w, &h);
	verify_wall_textures(cub);
}
