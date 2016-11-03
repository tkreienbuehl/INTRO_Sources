/**
 * \file
 * \brief Shell (command line) interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a command line interface to the application.
 */

#ifndef SHELL_H_
#define SHELL_H_

#include "Platform.h"
#if PL_CONFIG_HAS_SHELL
#include "CLS1.h"

void SHELL_SendChar(uint8_t ch);

/*!
 * \brief Parse a command string
 * \param cmd Zero terminated command to be parsed
 */
void SHELL_ParseCmd(unsigned char *cmd);

/*!
 * \brief Returns the default IO-Type
 */
CLS1_ConstStdIOType* SHELL_GetStdio(void);

/*!
 * \brief Sends a string to the shell/console stdout
 * \param msg Zero terminated string to write
 */
void SHELL_SendString(unsigned char *msg);

/*! \brief Shell Module initialization, creates Shell task */
void SHELL_Init(void);

/*! \brief Shell driver de-initialization */
void SHELL_Deinit(void);
#endif /* PL_CONFIG_HAS_SHELL */

#endif /* SHELL_H_ */
