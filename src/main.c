/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:20 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:22 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../includes/cub3d.h"
#include "../includes/parse_textures.h"
#include "../includes/parse_map.h"
#include "../includes/debug.h"
#include "../includes/map_utils.h"
#include "../includes/validate_map.h"
#include "../includes/utils.h"

void	assign_texture(t_cub3d *cub, char *line);
int		validate_file_extension(char *filename);
void	validate_textures(t_cub3d *cub);

int	is_whitespace_only(const char *line)
{
	int	i;

	if (!line)
		return (1);
	i = 0;
	while (line[i])
	{
		if (line[i] != ' ' && line[i] != '\t'
			&& line[i] != '\n' && line[i] != '\r')
			return (0);
		i++;
	}
	return (1);
}

static void	process_texture_or_color_line(t_cub3d *cub, char *line)
{
	if (is_texture_line(line, "NO ")
		|| is_texture_line(line, "SO ")
		|| is_texture_line(line, "WE ")
		|| is_texture_line(line, "EA "))
		assign_texture(cub, line);
	else if (is_color_line(line, 'F'))
		assign_color(&cub->textures.floor, line);
	else if (is_color_line(line, 'C'))
		assign_color(&cub->textures.ceiling, line);
	else
		ft_error("Invalid configuration line", cub, line);
}

static int	check_map_start(t_cub3d *cub, char *line)
{
	if (is_map_line(line))
	{
		if (!cub->textures.no || !cub->textures.so
			|| !cub->textures.we || !cub->textures.ea
			|| cub->textures.floor.r == -1
			|| cub->textures.ceiling.r == -1)
			ft_error(
				"Map found before all configuration elements are defined",
				cub,
				line
				);
		return (1);
	}
	return (0);
}

static char	*expand_line_buffer(char *line, int *cap, int len, t_cub3d *cub)
{
	char	*tmp;

	*cap *= 2;
	tmp = (char *)malloc(*cap);
	if (!tmp)
		ft_error("Memory allocation failed for line expansion", cub, line);
	ft_memcpy(tmp, line, len);
	free(line);
	return (tmp);
}

static void	handle_line_end(t_cub3d *cub, t_readmap_ctx *ctx)
{
	ctx->line[ctx->len] = '\0';
	if (ctx->line[0] != '\0' && !is_whitespace_only(ctx->line))
	{
		if (check_map_start(cub, ctx->line))
			ctx->stop = 1;
		else
			process_texture_or_color_line(cub, ctx->line);
	}
	ctx->len = 0;
	ctx->line[0] = '\0';
}

void	read_map(int fd, t_cub3d *cub)
{
	t_readmap_ctx	ctx;

	ctx.cap = 128;
	ctx.line = (char *)malloc(ctx.cap);
	ctx.len = 0;
	ctx.stop = 0;
	if (!ctx.line)
		ft_error("Memory allocation failed for line", cub, NULL);
	ctx.n = read(fd, &ctx.c, 1);
	while (!ctx.stop && ctx.n > 0)
	{
		if (ctx.c == '\n')
			handle_line_end(cub, &ctx);
		else
		{
			ctx.line[ctx.len++] = ctx.c;
			if (ctx.len + 1 >= ctx.cap)
				ctx.line = expand_line_buffer(ctx.line, &ctx.cap, ctx.len, cub);
		}
		ctx.n = read(fd, &ctx.c, 1);
	}
	free(ctx.line);
	print_textures_debug(cub);
	validate_textures(cub);
}

void	init_cub3d(t_cub3d *cub)
{
	ft_memset(cub, 0, sizeof(t_cub3d));
	cub->textures.floor.r = -1;
	cub->textures.floor.g = -1;
	cub->textures.floor.b = -1;
	cub->textures.ceiling.r = -1;
	cub->textures.ceiling.g = -1;
	cub->textures.ceiling.b = -1;
}

int	main(int argc, char *argv[])
{
	int		fd;
	t_cub3d	cub;

	init_cub3d(&cub);
	if (argc != 2)
	{
		fprintf(stderr, "Use: %s <map.cub>\n", argv[0]);
		return (1);
	}
	if (!validate_file_extension(argv[1]))
		ft_error("Invalid file extension. Please use a .cub file", NULL, NULL);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		ft_error("Cannot open file", NULL, NULL);
	read_map(fd, &cub);
	close(fd);
	validate_textures(&cub);
	assign_map(&cub, argv[1]);
	validate_map(&cub);
	print_map_debug(&cub);
	free_textures(&cub.textures);
	if (cub.map)
		free_map(cub.map, cub.map_height);
	return (0);
}
