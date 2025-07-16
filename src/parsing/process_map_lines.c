#include <stdlib.h>
#include <unistd.h>
#include "../../includes/cub3d.h"
#include "../../includes/parse_map.h"
#include "../../includes/map_utils.h"
#include "../../includes/utils.h"

char	*read_line(int fd);

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

static void	handle_map_line(t_cub3d *cub, t_map_parse_ctx *ctx, char *line)
{
	char	*trimmed_line;
	int		len;

	expand_temp_map_if_needed(cub, ctx, line);
	trimmed_line = trim_map_line_or_exit(cub, ctx, line);
	ctx->temp_map[ctx->map_lines] = trimmed_line;
	len = (int)ft_strlen(trimmed_line);
	if (len > cub->map_width)
		cub->map_width = len;
	ctx->map_lines++;
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

void	process_map_lines(t_cub3d *cub, int fd, t_map_parse_ctx *ctx)
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
