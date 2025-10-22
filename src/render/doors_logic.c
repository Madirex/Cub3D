#include "../../includes/cub3d_render.h"
#include <string.h>

/* Door logic: targeting and toggling */

/* Compute target cell coordinates at a given distance along player dir */
int get_target_cell_coords(t_cub3d *cub, double dist, int *tx, int *ty)
{
	*tx = (int)(cub->pos_x + cub->dir_x * dist);
	*ty = (int)(cub->pos_y + cub->dir_y * dist);
	if (*ty < 0 || *ty >= cub->map_height)
		return (0);
	if (*tx < 0 || *tx >= (int)strlen(cub->map[*ty]))
		return (0);
	return (1);
}

/* Toggle door char at map coordinates */
void toggle_door_at(t_cub3d *cub, int mx, int my)
{
	char	*cell;

	cell = &cub->map[my][mx];
	if (*cell == 'D')
		*cell = 'O';
	else if (*cell == 'O')
		*cell = 'D';
}

/* True if coordinates equal player's integer cell */
int is_same_as_player(t_cub3d *cub, int mx, int my)
{
	return ((int)cub->pos_x == mx && (int)cub->pos_y == my);
}

/* Public: handle user door action (open/close) */
void handle_door_action(t_cub3d *cub)
{
	int	target_mapX;
	int	target_mapY;

	if (!cub->is_bonus)
		return ;
	if (!get_target_cell_coords(cub, 1.0, &target_mapX, &target_mapY))
		return ;
	if (cub->map[target_mapY][target_mapX] == 'D')
	{
		toggle_door_at(cub, target_mapX, target_mapY);
		return ;
	}
	if (cub->map[target_mapY][target_mapX] == 'O')
	{
		if (is_same_as_player(cub, target_mapX, target_mapY))
			return ;
		toggle_door_at(cub, target_mapX, target_mapY);
	}
}