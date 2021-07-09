/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_liveliness.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cmeunier <cmeunier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/06 10:41:34 by celestin          #+#    #+#             */
/*   Updated: 2021/07/09 14:17:51 by cmeunier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../incs/philosophers.h"

static int	check_musteat(t_philosopher *philosopher)
{
	t_philosopher	*tmp;

	tmp = philosopher;
	while (tmp)
	{
		if (tmp->meals < tmp->settings->musteat)
			return (0);
		tmp = tmp->next;
	}
	pthread_mutex_lock(&philosopher->settings->mutex_musteat);
	philosopher->settings->musteat_max = 1;
	pthread_mutex_unlock(&philosopher->settings->mutex_musteat);
	return (1);
}

static int	check_starve(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->mutex_lastmeal);
	if ((long)get_time(philo->settings) - philo->lastmeal >= (long)philo->settings->time2die)
	{
		pthread_mutex_unlock(&philo->mutex_lastmeal);
		return (1);
	}
	pthread_mutex_unlock(&philo->mutex_lastmeal);
	return (0);
}

static void	check_death(t_philosopher *philosopher)
{
	t_philosopher	*tmp;

	tmp = philosopher;
	while (tmp)
	{
		if (check_starve(tmp) == 1)
		{
			printtime(get_time(philosopher->settings), tmp->id, "died", philosopher->settings);
			usleep(1000);
			pthread_mutex_lock(&philosopher->settings->mutex_alive);
			philosopher->settings->everyone_alive = 0;
			pthread_mutex_unlock(&philosopher->settings->mutex_alive);
			break ;
		}
		tmp = tmp->next;
	}
}

static void	*thread_liveliness(void *philosopher_cast)
{
	t_philosopher	*philosopher;

	philosopher = (t_philosopher *)philosopher_cast;
	while (philosophing_conditions(philosopher))
	{
		usleep(500);
		check_death(philosopher);
		if (philosopher->settings->option)
			check_musteat(philosopher);
	}
	return (NULL);
}

int	launch_liveliness_check(t_philosopher *philosopher)
{
	int				ret_pthread;
	t_philosopher	*tmp;
	pthread_t		*tid;

	tmp = philosopher;
	tid = &tmp->settings->tid_liveness_checker;
	ret_pthread = pthread_create(tid, NULL, thread_liveliness, (void *)tmp);
	if (ret_pthread != 0)
		return (ft_error("Unable to create thread."));
	return (0);
}
