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

#include "../../includes/cub3d.h"
#include "../../includes/utils.h"
#include "../../includes/parse_textures.h"

int		validate_texture_file(char *path);

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
