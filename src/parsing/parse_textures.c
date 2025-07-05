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
#include "../../includes/utils.h"

static void	set_texture(char **texture, char *line, int k)
{
	if (*texture)
		ft_error("Texture duplicated", NULL, NULL);
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
		set_texture(&cub->textures.no, line, i);
	else if (is_texture_line(line, "SO "))
		set_texture(&cub->textures.so, line, i);
	else if (is_texture_line(line, "WE "))
		set_texture(&cub->textures.we, line, i);
	else if (is_texture_line(line, "EA "))
		set_texture(&cub->textures.ea, line, i);
}

void	assign_color(t_rgb *color, char *line)
{
	int	i;

	if (color->r != -1 || color->g != -1 || color->b != -1)
		ft_error("Color duplicated", NULL, NULL);
	i = 2;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
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
		ft_error("Color value out of range (0-255)", NULL, NULL);
}
