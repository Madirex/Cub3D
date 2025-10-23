/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doors_loader.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:48:38 by skyce11           #+#    #+#             */
/*   Updated: 2025/10/23 03:57:34 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d_render.h"
#include "../includes/parse_textures.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void	safe_exit(t_cub3d *cub, char *line, int exit_code);

/*
** Initialize door_textures array and check malloc.
*/
void	init_door_textures_array(t_cub3d *cub)
{
	cub->door_textures = malloc(sizeof(*cub->door_textures) * 4);
	if (!cub->door_textures)
	{
		fprintf(stderr, "Error: malloc falló door_textures\n");
		safe_exit(cub, NULL, 1);
	}
}

/*
** Load the closed-door frames (up to 3 frames), fill fallbacks as needed.
*/
void	load_door_frames(t_cub3d *cub, void *mlx, int *w, int *h)
{
	int	tmp_w;
	int	tmp_h;

	tmp_w = *w;
	tmp_h = *h;
	cub->door_textures[0] = load_xpm_buffer(mlx,
			cub->textures.door_closed, &tmp_w, &tmp_h);
	if (cub->textures.door_closed_2)
		cub->door_textures[1] = load_xpm_buffer(mlx,
				cub->textures.door_closed_2, &tmp_w, &tmp_h);
	else
		cub->door_textures[1] = cub->door_textures[0];
	if (cub->textures.door_closed_3)
		cub->door_textures[2] = load_xpm_buffer(mlx,
				cub->textures.door_closed_3, &tmp_w, &tmp_h);
	else
		cub->door_textures[2] = cub->door_textures[0];
}

/*
** Set the open-door texture (index 3) or NULL if not provided.
*/
void	set_door_open_texture(t_cub3d *cub, void *mlx, int *tmp_w, int *tmp_h)
{
	if (cub->textures.door_open == NULL)
	{
		cub->door_textures[3] = NULL;
		return ;
	}
	cub->door_textures[3] = load_xpm_buffer(
			mlx, cub->textures.door_open, tmp_w, tmp_h);
}

/*
** Load door textures (animation frames) when bonus is enabled.
*/
void	load_doors(t_cub3d *cub, void *mlx, int *w, int *h)
{
	int	tmp_w;
	int	tmp_h;

	if (!cub->is_bonus || cub->textures.door_closed == NULL)
	{
		cub->door_textures = NULL;
		return ;
	}
	init_door_textures_array(cub);
	tmp_w = *w;
	tmp_h = *h;
	load_door_frames(cub, mlx, &tmp_w, &tmp_h);
	set_door_open_texture(cub, mlx, &tmp_w, &tmp_h);
	cub->door_anim_frame = 0;
	cub->door_anim_timer = 0.0;
	cub->door_anim_speed = 0.15;
	if (!cub->door_textures[0])
	{
		fprintf(stderr, "Error: door texture definida pero fallo al cargar\n");
		safe_exit(cub, NULL, 1);
	}
}
