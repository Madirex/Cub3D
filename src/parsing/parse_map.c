/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:35 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:36 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../includes/cub3d.h"
#include "../../includes/parse_map.h"
#include "../../includes/map_utils.h"
#include "../../includes/utils.h"

static char	*resize_line_buffer(char *line, int *cap, int len)
{
	char	*tmp;

	*cap *= 2;
	tmp = (char *)malloc(*cap);
	if (!tmp)
	{
		free(line);
		return (NULL);
	}
	ft_memcpy(tmp, line, len);
	free(line);
	return (tmp);
}

static char	*finalize_line(char *line, int n, int len)
{
	line[len] = '\0';
	if (n == 0 && len == 0)
	{
		free(line);
		return (NULL);
	}
	return (line);
}

static int	init_line_buffer(char **line, int *cap, int *len)
{
	*cap = 128;
	*len = 0;
	*line = (char *)malloc(*cap);
	if (!*line)
		return (0);
	return (1);
}

static char	*read_line(int fd)
{
	char	*line;
	char	c;
	int		len;
	int		cap;
	int		n;

	if (!init_line_buffer(&line, &cap, &len))
		return (NULL);
	n = read(fd, &c, 1);
	while (n > 0)
	{
		if (c == '\n')
			break ;
		if (c == '\r')
			continue ;
		line[len++] = c;
		if (len + 1 >= cap)
		{
			line = resize_line_buffer(line, &cap, len);
			if (!line)
				return (NULL);
		}
		n = read(fd, &c, 1);
	}
	return (finalize_line(line, n, len));
}

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

static char	*trim_map_line_or_exit(
		t_cub3d *cub, t_map_parse_ctx *ctx, char *line)
{
	char	*trimmed_line;

	trimmed_line = ft_strtrim(line, " \t\n\r");
	if (!trimmed_line)
	{
		free_map(ctx->temp_map, ctx->map_lines);
		ft_error("Memory allocation failed for map line trimming", cub, line);
	}
	return (trimmed_line);
}

static void	update_map_width(t_cub3d *cub, const char *trimmed_line)
{
	int	len;

	len = (int)ft_strlen(trimmed_line);
	if (len > cub->map_width)
		cub->map_width = len;
}

static void	handle_map_line(t_cub3d *cub, t_map_parse_ctx *ctx, char *line)
{
	char	*trimmed_line;

	expand_temp_map_if_needed(cub, ctx, line);
	trimmed_line = trim_map_line_or_exit(cub, ctx, line);
	ctx->temp_map[ctx->map_lines] = trimmed_line;
	update_map_width(cub, trimmed_line);
	ctx->map_lines++;
}

static void	finalize_map(t_cub3d *cub, t_map_parse_ctx *ctx)
{
	if (ctx->map_lines == 0)
	{
		free(ctx->temp_map);
		ft_error("No valid map found in the file", cub, NULL);
	}
	cub->map = ctx->temp_map;
	cub->map_height = ctx->map_lines;
}

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

static void	init_map_vars(t_cub3d *cub, t_map_parse_ctx *ctx)
{
	ctx->map_lines = 0;
	cub->map_width = 0;
	ctx->map_capacity = 5;
	ctx->temp_map = (char **)malloc(sizeof(char *) * (ctx->map_capacity));
	if (!ctx->temp_map)
		ft_error("Memory allocation failed for map", cub, NULL);
}

static int	open_map_file(const char *filename, t_cub3d *cub)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		ft_error("File could not be opened", cub, NULL);
	return (fd);
}

static void	process_map_lines(t_cub3d *cub, int fd, t_map_parse_ctx *ctx)
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
		if (is_map_line(line))
		{
			reading_map = 1;
			handle_map_line(cub, ctx, line);
		}
		else if (reading_map && !is_whitespace_only(line))
			ft_error("Invalid character found in map", cub, line);
		free(line);
		line = read_line(fd);
	}
}

void	assign_map(t_cub3d *cub, char *filename)
{
	t_map_parse_ctx	ctx;
	int				fd;

	init_map_vars(cub, &ctx);
	fd = open_map_file(filename, cub);
	process_map_lines(cub, fd, &ctx);
	close(fd);
	finalize_map(cub, &ctx);
}
