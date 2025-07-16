/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_loader.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:58:49 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 19:58:51 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file map_loader.c
 * @brief Map loading and parsing functions for the Cub3D project
 * 
 * This file contains functions for reading and parsing .cub files,
 * including texture and color configuration parsing.
 */

#include <stdlib.h>
#include <unistd.h>
#include "../../includes/parse_textures.h"
#include "../../includes/utils.h"
#include "../../includes/debug.h"
#include "../../includes/cub3d.h"
#include "../../includes/parse_map.h"

void	assign_texture(t_cub3d *cub, char *line);
void	validate_textures(t_cub3d *cub);

/**
 * @brief Processes a line containing texture or color configuration
 * 
 * Identifies and processes texture paths (NO, SO, WE, EA) and
 * color values (F for floor, C for ceiling).
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param line The line to process
 */
static void	process_texture_or_color_line(t_cub3d *cub, char *line)
{
	if (is_texture_line(line, "NO ")
		|| is_texture_line(line, "SO ")
		|| is_texture_line(line, "WE ")
		|| is_texture_line(line, "EA "))
		assign_texture(cub, line);
	else if (is_color_line(line, 'F'))
		assign_color(&cub->textures.floor, line);
	else if (is_color_line(line, 'C'))
		assign_color(&cub->textures.ceiling, line);
	else
		ft_error("Invalid configuration line", cub, line);
}

/**
 * @brief Checks if a line marks the start of the map section
 * 
 * Validates that all required configuration elements (textures and colors)
 * are defined before the map section begins.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param line The line to check
 * @return 1 if this line starts the map, 0 otherwise
 */
static int	check_map_start(t_cub3d *cub, char *line)
{
	if (is_map_line(line))
	{
		if (!cub->textures.no || !cub->textures.so
			|| !cub->textures.we || !cub->textures.ea
			|| cub->textures.floor.r == -1
			|| cub->textures.ceiling.r == -1)
			ft_error(
				"Map found before all configuration elements are defined",
				cub,
				line
				);
		return (1);
	}
	return (0);
}

/**
 * @brief Handles the end of a line during file reading
 * 
 * Null-terminates the current line, processes it if it's not empty
 * or whitespace-only, and resets the line buffer for the next line.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param ctx Pointer to the reading context
 */
static void	handle_line_end(t_cub3d *cub, t_readmap_ctx *ctx)
{
	ctx->line[ctx->len] = '\0';
	if (ctx->line[0] != '\0' && !is_whitespace_only(ctx->line))
	{
		if (check_map_start(cub, ctx->line))
			ctx->stop = 1;
		else
			process_texture_or_color_line(cub, ctx->line);
	}
	ctx->len = 0;
	ctx->line[0] = '\0';
}

/**
 * @brief Expands the line buffer when capacity is exceeded
 * 
 * Doubles the buffer capacity, allocates new memory, copies existing
 * content, and frees the old buffer.
 * 
 * @param line Current line buffer
 * @param cap Pointer to current capacity (will be doubled)
 * @param len Current length of content in buffer
 * @param cub Pointer to the main Cub3D structure (for error handling)
 * @return New expanded buffer
 */
static char	*expand_line_buffer(char *line, int *cap, int len, t_cub3d *cub)
{
	char	*tmp;

	*cap *= 2;
	tmp = (char *)malloc(*cap);
	if (!tmp)
		ft_error("Memory allocation failed for line expansion", cub, line);
	ft_memcpy(tmp, line, len);
	free(line);
	return (tmp);
}

/**
 * @brief Main function to read and parse a .cub file
 * 
 * Reads the file character by character, building lines and processing
 * texture/color configuration until the map section is reached.
 * Handles dynamic buffer expansion and validates textures after parsing.
 * 
 * @param fd File descriptor of the .cub file to read
 * @param cub Pointer to the main Cub3D structure to populate
 */
void	read_map(int fd, t_cub3d *cub)
{
	t_readmap_ctx	ctx;

	ctx.cap = 128;
	ctx.line = (char *)malloc(ctx.cap);
	ctx.len = 0;
	ctx.stop = 0;
	if (!ctx.line)
		ft_error("Memory allocation failed for line", cub, NULL);
	ctx.n = read(fd, &ctx.c, 1);
	while (!ctx.stop && ctx.n > 0)
	{
		if (ctx.c == '\n')
			handle_line_end(cub, &ctx);
		else
		{
			ctx.line[ctx.len++] = ctx.c;
			if (ctx.len + 1 >= ctx.cap)
				ctx.line = expand_line_buffer(ctx.line, &ctx.cap, ctx.len, cub);
		}
		ctx.n = read(fd, &ctx.c, 1);
	}
	free(ctx.line);
	print_textures_debug(cub);
	validate_textures(cub);
}
