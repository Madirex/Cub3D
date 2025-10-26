/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <anmateo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 11:24:41 by anmateo-          #+#    #+#             */
/*   Updated: 2025/10/23 07:28:24 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"
#include <stdlib.h>

/**
 * @brief Free a pointer and set it to NULL
 * 
 * @param ptr Double pointer to the memory to free
 */
static void	free_and_null(void **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}

/**
 * @brief Frees wall texture buffers
 * 
 * Frees the pixel buffers for all four wall textures
 * and then frees the array itself.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
static void	free_wall_textures(t_cub3d *cub)
{
	int	i;

	if (!cub->wall_textures)
		return ;
	i = 0;
	while (i < 4)
	{
		if (cub->wall_textures[i])
			free_and_null((void **)&cub->wall_textures[i]);
		i++;
	}
	free_and_null((void **)&cub->wall_textures);
}

/**
 * @brief Frees door texture buffers
 * 
 * Carefully frees door texture buffers avoiding double-free
 * when multiple indices point to the same buffer.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
static void	free_door_textures(t_cub3d *cub)
{
	if (!cub->door_textures)
		return ;
	if (cub->door_textures[0])
		free_and_null((void **)&cub->door_textures[0]);
	if (cub->door_textures[1] && cub->door_textures[1] != cub->door_textures[0])
		free_and_null((void **)&cub->door_textures[1]);
	if (cub->door_textures[2] && cub->door_textures[2] != cub->door_textures[0])
		free_and_null((void **)&cub->door_textures[2]);
	if (cub->door_textures[3])
		free_and_null((void **)&cub->door_textures[3]);
	free_and_null((void **)&cub->door_textures);
}

/**
 * @brief Frees all texture paths including bonus textures
 * 
 * Extended version that frees all texture paths,
 * including door textures for bonus mode.
 * 
 * @param textures Pointer to the textures structure
 */
static void	free_textures(t_textures *textures)
{
	if (textures->no)
		free_and_null((void **)&textures->no);
	if (textures->so)
		free_and_null((void **)&textures->so);
	if (textures->we)
		free_and_null((void **)&textures->we);
	if (textures->ea)
		free_and_null((void **)&textures->ea);
	if (textures->door_closed)
		free_and_null((void **)&textures->door_closed);
	if (textures->door_open)
		free_and_null((void **)&textures->door_open);
	if (textures->door_closed_2)
		free_and_null((void **)&textures->door_closed_2);
	if (textures->door_closed_3)
		free_and_null((void **)&textures->door_closed_3);
}

/**
 * @brief Complete cleanup of all texture-related memory
 * 
 * Master function that calls all texture cleanup functions
 * in the correct order.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
void	cleanup_all_textures(t_cub3d *cub)
{
	if (!cub)
		return ;
	free_wall_textures(cub);
	free_door_textures(cub);
	free_textures(&cub->textures);
}
