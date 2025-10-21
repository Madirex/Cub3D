/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assign_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 18:50:41 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 18:50:41 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file assign_utils.c
 * @brief Texture assignment utility functions for the Cub3D project
 * 
 * This file contains functions for assigning texture paths
 * from parsed configuration lines with validation.
 */

#include "../../includes/cub3d.h"
#include "../../includes/utils.h"
#include "../../includes/parse_textures.h"

int		validate_texture_file(char *path);

/**
 * @brief Sets a texture path after validation
 * 
 * Extracts the texture path from a line, trims whitespace,
 * validates the file exists, and assigns it to the texture pointer.
 * 
 * @param texture Pointer to the texture string pointer
 * @param line The line containing the texture path
 * @param k Starting index in the line
 */
static void	set_texture(char **texture, char *line, int k)
{
	char	*path;
	char	*trimmed_path;

	if (*texture)
		ft_error("Texture duplicated", NULL, NULL);
	path = ft_strdup(line + k);
	if (!path)
		ft_error("Memory allocation failed for texture path", NULL, NULL);
	trimmed_path = ft_strtrim(path, " \t\n\r");
	free(path);
	if (!trimmed_path)
		ft_error("Memory allocation failed for texture path", NULL, NULL);
	if (!validate_texture_file(trimmed_path))
	{
		free(trimmed_path);
		ft_error("Texture file not found or cannot be opened", NULL, NULL);
	}
	*texture = trimmed_path;
}

/**
 * @brief Helper function to find the starting position of texture path in a line
 * 
 * Skips whitespace and the texture identifier to find where the
 * actual texture path starts.
 * 
 * @param line The line containing the texture configuration
 * @return Index of the first character of the texture path
 */
static int	find_texture_path_start(const char *line)
{
	int	i;

	if (!line)
		return (0);
	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	i += 2;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	return (i);
}

/**
 * @brief Assigns a texture based on the line identifier
 * 
 * Parses the line to identify which texture (NO, SO, WE, EA)
 * is being defined and assigns the corresponding texture path.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param line The line containing texture information
 */
void	assign_texture(t_cub3d *cub, char *line)
{
	int	i;

	if (!line)
		return ;
	i = find_texture_path_start(line);
	if (is_texture_line(line, "NO "))
		set_texture(&cub->textures.no, line, i);
	else if (is_texture_line(line, "SO "))
		set_texture(&cub->textures.so, line, i);
	else if (is_texture_line(line, "WE "))
		set_texture(&cub->textures.we, line, i);
	else if (is_texture_line(line, "EA "))
		set_texture(&cub->textures.ea, line, i);
	else if (cub->is_bonus && is_texture_line(line, "DC "))
		set_texture(&cub->textures.door_closed, line, i);
	else if (cub->is_bonus && is_texture_line(line, "DO "))
		set_texture(&cub->textures.door_open, line, i);
	else if (cub->is_bonus && is_texture_line(line, "D2 "))
		set_texture(&cub->textures.door_closed_2, line, i);
	else if (cub->is_bonus && is_texture_line(line, "D3 "))
		set_texture(&cub->textures.door_closed_3, line, i);
	else
		ft_error("Invalid texture identifier", cub, line);
}
