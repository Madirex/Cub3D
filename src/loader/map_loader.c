/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_loader.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:58:49 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 19:58:51 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include "../../includes/parse_textures.h"
#include "../../includes/utils.h"
#include "../../includes/debug.h"
#include "../../includes/cub3d.h"
#include "../../includes/parse_map.h"

void	assign_texture(t_cub3d *cub, char *line);
int		is_whitespace_only(const char *line);
void	validate_textures(t_cub3d *cub);

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
