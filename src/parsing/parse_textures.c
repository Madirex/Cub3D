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

void	assign_texture(t_cub3d *cub, char *line)
{
	int	i;

	if (!line)
		return ;
	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	i += 2;
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
