/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:35 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:36 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file parse_map.c
 * @brief Map parsing functions for the Cub3D project
 * 
 * This file contains functions for parsing map data from .cub files,
 * including initialization, processing, and finalization of map data.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../includes/cub3d.h"
#include "../../includes/parse_map.h"
#include "../../includes/map_utils.h"
#include "../../includes/utils.h"

void	process_map_lines(t_cub3d *cub, int fd, t_map_parse_ctx *ctx);

/**
 * @brief Initializes map parsing variables and allocates initial memory
 * 
 * Sets up the parsing context with initial values and allocates
 * memory for the temporary map storage.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param ctx Pointer to the map parsing context
 */
static void	init_map_vars(t_cub3d *cub, t_map_parse_ctx *ctx)
{
	ctx->map_lines = 0;
	cub->map_width = 0;
	ctx->map_capacity = 5;
	ctx->temp_map = (char **)malloc(sizeof(char *) * (ctx->map_capacity));
	if (!ctx->temp_map)
		ft_error("Memory allocation failed for map", cub, NULL);
}

/**
 * @brief Finalizes map parsing and assigns the map to the main structure
 * 
 * Validates that a map was found and assigns the temporary map
 * to the main Cub3D structure.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param ctx Pointer to the map parsing context
 */
static void	finalize_map(t_cub3d *cub, t_map_parse_ctx *ctx)
{
	if (ctx->map_lines == 0)
	{
		free(ctx->temp_map);
		ft_error("No valid map found in the file", cub, NULL);
	}
	cub->map = ctx->temp_map;
	cub->map_height = ctx->map_lines;
}

/**
 * @brief Main function to assign map data from a .cub file
 * 
 * Opens the file, processes all map lines, and finalizes
 * the map assignment to the Cub3D structure.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param filename Path to the .cub file
 */
void	assign_map(t_cub3d *cub, char *filename)
{
	t_map_parse_ctx	ctx;
	int				fd;

	init_map_vars(cub, &ctx);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		ft_error("File could not be opened", cub, NULL);
	process_map_lines(cub, fd, &ctx);
	close(fd);
	finalize_map(cub, &ctx);
}
