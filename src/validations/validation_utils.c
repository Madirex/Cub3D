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

#include <fcntl.h>
#include <unistd.h>
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
