#include "../../includes/cub3d_render.h"
#include <string.h>
#include <math.h>

/* Find player start on the map and initialize player state */

/* Search map for player start and set pos+dir */
int find_player_position(t_cub3d *cub)
{
	int	y;
	int	x;

	y = 0;
	while (y < cub->map_height)
	{
		x = 0;
		while (x < (int)strlen(cub->map[y]))
		{
			if (cub->map[y][x] == 'N' || cub->map[y][x] == 'S'
				|| cub->map[y][x] == 'E' || cub->map[y][x] == 'W')
			{
				cub->pos_x = x + 0.5;
				cub->pos_y = y + 0.5;
				set_player_dir(cub, cub->map[y][x]);
				return (1);
			}
			x++;
		}
		y++;
	}
	return (0);
}

/* Public init wrapper */
void init_player(t_cub3d *cub)
{
	(void)find_player_position;
	find_player_position(cub);
}

/* Rotate player by angle (used by mouse handler) */
void rotate_player(t_cub3d *cub, double angle)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(angle)
		- cub->dir_y * sin(angle);
	cub->dir_y = old_dir_x * sin(angle)
		+ cub->dir_y * cos(angle);
	old_plane_x = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(angle)
		- cub->plane_y * sin(angle);
	cub->plane_y = old_plane_x * sin(angle)
		+ cub->plane_y * cos(angle);
}