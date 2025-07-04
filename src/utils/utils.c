/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:48 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:49 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "../../includes/cub3d.h"

void	free_textures(t_textures *textures)
{
	if (textures->no)
		free(textures->no);
	if (textures->so)
		free(textures->so);
	if (textures->we)
		free(textures->we);
	if (textures->ea)
		free(textures->ea);
}

void	free_map(char **map, int height)
{
	int	i;

	if (!map)
		return ;
	i = 0;
	while (i < height)
	{
		if (map[i])
			free(map[i]);
		i++;
	}
	free(map);
}

void	safe_exit(t_cub3d *cub, char *line)
{
	if (cub)
	{
		free_textures(&cub->textures);
		if (cub->map)
			free_map(cub->map, cub->map_height);
	}
	if (line)
		free(line);
	exit(1);
}
