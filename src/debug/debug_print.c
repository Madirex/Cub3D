/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:13 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:15 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "../../includes/cub3d.h"

void print_textures_debug(t_cub3d *cub)
{
	printf("[Debug] Textures:\n");
	if (cub->textures.no)
		printf("NO: %s\n", cub->textures.no);
	if (cub->textures.so)
		printf("SO: %s\n", cub->textures.so);
	if (cub->textures.we)
		printf("WE: %s\n", cub->textures.we);
	if (cub->textures.ea)
		printf("EA: %s\n", cub->textures.ea);
	if (cub->textures.floor.r != -1)
		printf("Floor: %d,%d,%d\n", cub->textures.floor.r, cub->textures.floor.g, cub->textures.floor.b);
	if (cub->textures.ceiling.r != -1)
		printf("Ceiling: %d,%d,%d\n", cub->textures.ceiling.r, cub->textures.ceiling.g, cub->textures.ceiling.b);
}

void print_map_debug(t_cub3d *cub)
{
	printf("[Debug] Map (%dx%d):\n", cub->map_width, cub->map_height);
	if (!cub->map)
	{
		printf("No map loaded\n");
		return;
	}
	
	int i = 0;
	while (i < cub->map_height)
	{
		printf("%2d: %s\n", i, cub->map[i]);
		i++;
	}
	printf("\n");
}
