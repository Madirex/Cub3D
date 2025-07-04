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

#include "../../includes/cub3d.h"

static void	set_texture(char **texture, char *line, int k, const char *type)
{
	if (*texture)
	{
		fprintf(stderr, "Error: %s duplicated\n", type);
		exit(1);
	}
	*texture = ft_strdup(line + k);
}

int	is_texture_line(const char *line, const char *id)
{
	return (ft_strncmp(line, id, 3) == 0);
}

int	is_color_line(const char *line, char id)
{
	return (line[0] == id && line[1] == ' ');
}

void	assign_texture(t_cub3d *cub, char *line)
{
	int	i;

	i = 3;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	if (is_texture_line(line, "NO "))
		set_texture(&cub->textures.no, line, i, "NO");
	else if (is_texture_line(line, "SO "))
		set_texture(&cub->textures.so, line, i, "SO");
	else if (is_texture_line(line, "WE "))
		set_texture(&cub->textures.we, line, i, "WE");
	else if (is_texture_line(line, "EA "))
		set_texture(&cub->textures.ea, line, i, "EA");
}

void	assign_color(t_rgb *color, char *line, const char *type)
{
	int	i;

	if (color->r != -1 || color->g != -1 || color->b != -1)
	{
		fprintf(stderr, "Error: %s duplicated\n", type);
		exit(1);
	}
	i = 2;
	color->r = ft_atoi(&line[i]);
	while (line[i] && line[i] != ',')
		i++;
	if (line[i] == ',')
		i++;
	color->g = ft_atoi(&line[i]);
	while (line[i] && line[i] != ',')
		i++;
	if (line[i] == ',')
		i++;
	color->b = ft_atoi(&line[i]);
	if (color->r < 0 || color->r > 255
		|| color->g < 0 || color->g > 255
		|| color->b < 0 || color->b > 255)
	{
		fprintf(stderr, "Error: %s color out of range (0-255)\n", type);
		exit(1);
	}
}
