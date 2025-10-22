/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_map_lines.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <anmateo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 11:52:22 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/16 11:52:22 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file process_map_lines.c
 * @brief Map line processing functions for the Cub3D project
 * 
 * This file contains functions for processing individual map lines,
 * including memory management and validation during map parsing.
 */

#include <stdlib.h>
#include <unistd.h>
#include "../../includes/cub3d.h"
#include "../../includes/parse_map.h"
#include "../../includes/map_utils.h"
#include "../../includes/utils.h"

char	*read_line(int fd);

/**
 * @brief Expands the temporary map array if needed
 * 
 * Doubles the capacity of the temporary map array when
 * the current capacity is exceeded.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param ctx Pointer to the map parsing context
 * @param line Current line being processed (for error handling)
 */
static void	expand_temp_map_if_needed(
		t_cub3d *cub, t_map_parse_ctx *ctx, char *line)
{
	char	**new_temp;
	int		i;

	if (ctx->map_lines >= ctx->map_capacity)
	{
		ctx->map_capacity *= 2;
		new_temp = (char **)malloc(sizeof(char *) * (ctx->map_capacity));
		if (!new_temp)
		{
			free_map(ctx->temp_map, ctx->map_lines);
			ft_error("Memory allocation failed for map expansion", cub, line);
		}
		i = 0;
		while (i < ctx->map_lines)
		{
			new_temp[i] = ctx->temp_map[i];
			i++;
		}
		free(ctx->temp_map);
		ctx->temp_map = new_temp;
	}
}

/**
 * @brief Removes newline and carriage return characters
 * from the end of a string.
 */
static char	*trim_newline_chars(char *str)
{
	int	len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
	{
		str[len - 1] = '\0';
		len--;
	}
	return (str);
}

/**
 * @brief Handles processing of a single map line
 * 
 * Expands the map array if needed, trims the line, and
 * updates the map width if necessary.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param ctx Pointer to the map parsing context
 * @param line The map line to process
 */
static void	handle_map_line(t_cub3d *cub, t_map_parse_ctx *ctx, char *line)
{
	int	len;

	expand_temp_map_if_needed(cub, ctx, line);
	trim_newline_chars(line);
	ctx->temp_map[ctx->map_lines] = line;
	len = (int)ft_strlen(line);
	if (len > cub->map_width)
		cub->map_width = len;
	ctx->map_lines++;
}

/**
 * @brief Checks that no content exists after the map ends
 * 
 * Validates that the map is continuous and no non-whitespace
 * content appears after the map section.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param fd File descriptor to read from
 * @param ctx Pointer to the map parsing context
 * @param line Current line being processed
 */
static void	check_no_content_after_map(
			t_cub3d *cub, int fd, t_map_parse_ctx *ctx, char *line)
{
	char	*next_line;

	next_line = read_line(fd);
	while (next_line != NULL)
	{
		if (!is_whitespace_only(next_line))
		{
			free(line);
			free(next_line);
			free_map(ctx->temp_map, ctx->map_lines);
			ft_error(
				"Content found after map ended. Map must be continuous.",
				cub,
				NULL);
		}
		free(next_line);
		next_line = read_line(fd);
	}
	free(line);
}

/**
 * @brief Main function to process all map lines from a file
 * 
 * Reads lines from the file descriptor and processes them
 * as map data, ensuring continuity and proper formatting.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param fd File descriptor to read from
 * @param ctx Pointer to the map parsing context
 */
void	process_map_lines(t_cub3d *cub, int fd, t_map_parse_ctx *ctx)
{
	int		reading_map;
	char	*line;

	reading_map = 0;
	line = read_line(fd);
	while (line != NULL)
	{
		if (reading_map && is_whitespace_only(line))
		{
			check_no_content_after_map(cub, fd, ctx, line);
			break ;
		}
		if (is_map_line(line, cub))
		{
			reading_map = 1;
			handle_map_line(cub, ctx, line);
		}
		else if (reading_map && !is_whitespace_only(line))
			ft_error("Invalid character found in map", cub, line);
		else
			free(line);
		line = read_line(fd);
	}
}
