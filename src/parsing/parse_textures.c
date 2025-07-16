/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_textures.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:41 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:42 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file parse_textures.c
 * @brief Texture and color parsing functions for the Cub3D project
 * 
 * This file contains functions for parsing RGB color values
 * from .cub configuration files with proper validation.
 */

#include <fcntl.h>
#include "../../includes/cub3d.h"
#include "../../includes/utils.h"

int		is_texture_line(const char *line, const char *id);
int		is_color_line(const char *line, char id);

/**
 * @brief Skips whitespace characters in a line
 * 
 * @param line The line to process
 * @param i Starting index
 * @return Index after skipping whitespace
 */
static int	skip_spaces(const char *line, int i)
{
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	return (i);
}

/**
 * @brief Counts the number of commas in a line starting from index i
 * 
 * @param line The line to process
 * @param i Starting index
 * @return Number of commas found
 */
static int	count_commas(const char *line, int i)
{
	int	count;

	count = 0;
	while (line[i] && line[i] != '\n')
	{
		if (line[i] == ',')
			count++;
		i++;
	}
	return (count);
}

/**
 * @brief Skips identifier and surrounding spaces
 * 
 * @param line The line to process
 * @param i Starting index
 * @return Index after skipping identifier and spaces
 */
static int	skip_id_and_spaces(const char *line, int i)
{
	i = skip_spaces(line, i);
	i++;
	i = skip_spaces(line, i);
	return (i);
}

/**
 * @brief Parses a single RGB color component from a line
 * 
 * @param line The line containing the color value
 * @param i Pointer to the current index (updated after parsing)
 * @return The parsed color value
 */
static int	parse_color_component(const char *line, int *i)
{
	int	value;

	*i = skip_spaces(line, *i);
	if (!ft_isdigit(line[*i]))
		ft_error("Invalid color format. Expected: R,G,B", NULL, NULL);
	value = ft_atoi(&line[*i]);
	while (line[*i] && ft_isdigit(line[*i]))
		(*i)++;
	return (value);
}

/**
 * @brief Assigns RGB color values from a parsed line
 * 
 * Parses a line containing RGB values in format "R,G,B" and
 * assigns them to the provided RGB structure with validation.
 * 
 * @param color Pointer to the RGB structure to fill
 * @param line The line containing color information
 */
void	assign_color(t_rgb *color, char *line)
{
	int	i;
	int	comma_count;

	if (color->r != -1 || color->g != -1 || color->b != -1)
		ft_error("Color duplicated", NULL, NULL);
	i = skip_id_and_spaces(line, 0);
	if (!line[i] || line[i] == '\n')
		ft_error("Missing color values", NULL, NULL);
	comma_count = count_commas(line, i);
	if (comma_count != 2)
		ft_error("Invalid color format. Expected: R,G,B", NULL, NULL);
	color->r = parse_color_component(line, &i);
	i = skip_spaces(line, i);
	if (line[i] != ',')
		ft_error("Invalid color format. Expected: R,G,B", NULL, NULL);
	i++;
	color->g = parse_color_component(line, &i);
	i = skip_spaces(line, i);
	if (line[i] != ',')
		ft_error("Invalid color format. Expected: R,G,B", NULL, NULL);
	i++;
	color->b = parse_color_component(line, &i);
	if (color->r < 0 || color->r > 255 || color->g < 0
		|| color->g > 255 || color->b < 0 || color->b > 255)
		ft_error("Color value out of range (0-255)", NULL, NULL);
}
