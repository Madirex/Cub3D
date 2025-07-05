#include "../../includes/cub3d.h"
#include "../../includes/utils.h"

static int	is_player_char(char c)
{
	return (c == 'N' || c == 'S' || c == 'E' || c == 'W');
}

static void	find_player_position(t_cub3d *cub)
{
	int	i;
	int	j;
	int	player_count;

	player_count = 0;
	i = 0;
	while (i < cub->map_height)
	{
		j = 0;
		while (j < (int)ft_strlen(cub->map[i]))
		{
			if (is_player_char(cub->map[i][j]))
			{
				cub->player_x = j;
				cub->player_y = i;
				cub->player_dir = cub->map[i][j];
				player_count++;
			}
			j++;
		}
		i++;
	}
	if (player_count != 1)
		ft_error("Map must contain exactly one player", cub, NULL);
}

static int	is_border_position(t_cub3d *cub, int x, int y)
{
	return (y == 0 || y == cub->map_height - 1 || 
			x == 0 || x >= (int)ft_strlen(cub->map[y]) - 1);
}

static void	flood_fill(t_cub3d *cub, int **visited, int x, int y)
{
	if (y < 0 || y >= cub->map_height || x < 0)
		return ;
	if (x >= (int)ft_strlen(cub->map[y]))
		return ;
	if (visited[y][x] || cub->map[y][x] == '1')
		return ;
	if (cub->map[y][x] == ' ')
	{
		if (is_border_position(cub, x, y))
			return ;
	}
	if (is_border_position(cub, x, y) && cub->map[y][x] != '1' && cub->map[y][x] != ' ')
		ft_error("Map is not closed by walls", cub, NULL);
	visited[y][x] = 1;
	flood_fill(cub, visited, x + 1, y);
	flood_fill(cub, visited, x - 1, y);
	flood_fill(cub, visited, x, y + 1);
	flood_fill(cub, visited, x, y - 1);
}

void	validate_map(t_cub3d *cub)
{
	int	**visited;
	int	i;

	if (!cub->map || cub->map_height == 0)
		ft_error("No map found", cub, NULL);
	find_player_position(cub);
	visited = (int **)malloc(sizeof(int *) * cub->map_height);
	if (!visited)
		ft_error("Memory allocation failed for map validation", cub, NULL);
	
	i = 0;
	while (i < cub->map_height)
	{
		visited[i] = (int *)ft_calloc(cub->map_width, sizeof(int));
		if (!visited[i])
		{
			while (--i >= 0)
				free(visited[i]);
			free(visited);
			ft_error("Memory allocation failed for map validation", cub, NULL);
		}
		i++;
	}
	flood_fill(cub, visited, cub->player_x, cub->player_y);
	i = 0;
	while (i < cub->map_height)
	{
		free(visited[i]);
		i++;
	}
	free(visited);
}
