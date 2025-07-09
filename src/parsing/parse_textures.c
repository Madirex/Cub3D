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

#include <fcntl.h>
#include "../../includes/cub3d.h"
#include "../../includes/utils.h"

int		is_texture_line(const char *line, const char *id);
int		is_color_line(const char *line, char id);

static int	skip_spaces(const char *line, int i)
{
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	return (i);
}

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

static int	skip_id_and_spaces(const char *line, int i)
{
	i = skip_spaces(line, i);
	i++;
	i = skip_spaces(line, i);
	return (i);
}

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
