/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wall_loader.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 12:48:38 by skyce11           #+#    #+#             */
/*   Updated: 2025/10/23 03:28:01 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d_render.h"
#include "../includes/parse_textures.h"
#include <stdlib.h>
#include <stdio.h>

void	safe_exit(t_cub3d *cub, char *line, int exit_code);

/*
** Load the four wall textures and return their size via w/h.
*/
void	load_walls(t_cub3d *cub, void *mlx, int *w, int *h)
{
	int	tmp_w;
	int	tmp_h;

	tmp_w = 0;
	tmp_h = 0;
	cub->wall_textures = malloc(sizeof(*cub->wall_textures) * 4);
	if (!cub->wall_textures)
	{
		fprintf(stderr, "Error: malloc falló wall_textures\n");
		safe_exit(cub, NULL, 1);
	}
	cub->wall_textures[0] = load_xpm_buffer(mlx, cub->textures.no,
			&tmp_w, &tmp_h);
	cub->wall_textures[1] = load_xpm_buffer(mlx, cub->textures.so,
			&tmp_w, &tmp_h);
	cub->wall_textures[2] = load_xpm_buffer(mlx, cub->textures.we,
			&tmp_w, &tmp_h);
	cub->wall_textures[3] = load_xpm_buffer(mlx, cub->textures.ea,
			&tmp_w, &tmp_h);
	*w = tmp_w;
	*h = tmp_h;
}

/*
** Verify that wall textures were loaded; abort on failure.
*/
void	verify_wall_textures(t_cub3d *cub)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		if (!cub->wall_textures[i])
		{
			fprintf(stderr, "Error: wall texture %d not loaded\n", i);
			safe_exit(cub, NULL, 1);
		}
		i++;
	}
}
