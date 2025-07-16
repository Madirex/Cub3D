/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 18:47:36 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 18:47:37 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file validation_utils.c
 * @brief Validation utility functions for the Cub3D project
 * 
 * This file contains utility functions for validating file extensions,
 * texture files, and configuration line formats.
 */

#include <fcntl.h>
#include <unistd.h>
#include "../../includes/utils.h"

/**
 * @brief Validates that a texture file exists and has correct extension
 * 
 * Checks that the file has .xpm extension and can be opened for reading.
 * 
 * @param path Path to the texture file
 * @return 1 if file is valid, 0 otherwise
 */
int	validate_texture_file(char *path)
{
	int	fd;
	int	len;

	if (!path)
		return (0);
	len = ft_strlen(path);
	if (len < 4)
		return (0);
	if (ft_strncmp(path + len - 4, ".xpm", 4) != 0)
		return (0);
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	close(fd);
	return (1);
}

/**
 * @brief Checks if a line contains texture information
 * 
 * Validates that the line starts with the specified texture identifier
 * followed by whitespace.
 * 
 * @param line The line to check
 * @param id The texture identifier (NO, SO, WE, EA)
 * @return 1 if it's a texture line, 0 otherwise
 */
int	is_texture_line(const char *line, const char *id)
{
	int	i;
	int	j;

	if (!line || !id)
		return (0);
	i = 0;
	j = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	while (j < 2 && id[j] && line[i])
	{
		if (line[i] != id[j])
			return (0);
		i++;
		j++;
	}
	return (j == 2 && (line[i] == ' ' || line[i] == '\t'));
}

/**
 * @brief Checks if a line contains color information
 * 
 * Validates that the line starts with the specified color identifier
 * followed by whitespace.
 * 
 * @param line The line to check
 * @param id The color identifier (F for floor, C for ceiling)
 * @return 1 if it's a color line, 0 otherwise
 */
int	is_color_line(const char *line, char id)
{
	int	i;

	if (!line)
		return (0);
	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	return (line[i] == id && (line[i + 1] == ' ' || line[i + 1] == '\t'));
}

/**
 * @brief Validates that a filename has the correct .cub extension
 * 
 * @param filename The filename to validate
 * @return 1 if filename has .cub extension, 0 otherwise
 */
int	validate_file_extension(char *filename)
{
	int	len;

	len = ft_strlen(filename);
	if (len < 4)
		return (0);
	if (ft_strncmp(filename + len - 4, ".cub", 4) != 0)
		return (0);
	return (1);
}

/**
 * @brief Validates that all required textures and colors are defined
 * 
 * Checks that all four wall textures (NO, SO, WE, EA) and both
 * colors (floor and ceiling) have been properly configured.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
void	validate_textures(t_cub3d *cub)
{
	if (!cub->textures.no || !cub->textures.so || !cub->textures.we
		|| !cub->textures.ea || cub->textures.floor.r == -1
		|| cub->textures.floor.g == -1 || cub->textures.floor.b == -1
		|| cub->textures.ceiling.r == -1
		|| cub->textures.ceiling.g == -1
		|| cub->textures.ceiling.b == -1)
		ft_error("Missing required fields in .cub file", cub, NULL);
}
