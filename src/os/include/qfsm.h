/*This file is part of the QuarkTS distribution.*/
#ifndef QFSM_H
    #define QFSM_H

    #include "qtypes.h"
    #include "qqueues.h"

    #ifdef __cplusplus
    extern "C" {
    #endif

    typedef enum {qSM_EXIT_SUCCESS = -32768, qSM_EXIT_FAILURE = -32767} qSM_Status_t;
    #define _qSM_Handler_t struct _qSM_PublicData_s * 
    #define CurrentState    NextState
  
    typedef size_t qSM_Signal_t;

    typedef struct _qSM_PublicData_s{
        /* NextState: (Read/Write) 
        Next state to be performed after this state finish
        */
        qSM_Status_t (*NextState)(_qSM_Handler_t arg);
        /* PreviousState: (Read Only)
        Last state seen in the flow chart
        */
        qSM_Status_t (* PreviousState)(_qSM_Handler_t arg);
        /* LastState: (Read Only) 
        The last state executed
        */        
        qSM_Status_t (* LastState)(_qSM_Handler_t arg);
        /* Data: (Read Only)
        State-machine associated data.
        Note: If the FSM is running as a task, the associated event data can be 
        queried throught the "Data" field. (cast to qEvent_t is mandatory)
        */
        void * Data;
        /* Signal: (Read Only)
        Received signal from the transition table queue.
        QSM_SIGNAL_NONE if no table or signal available.
        QSM_SIGNAL_ENTRY When the current state has just entry
        QSM_SIGNAL_EXIT when the current state is exitimg after a transition
        */
        qSM_Signal_t Signal;
        /* PreviousReturnStatus: (Read Only)
        The return status of <PreviousState>
        */
        qSM_Status_t PreviousReturnStatus;
        /* LastReturnStatus: (Read Only)
        The return status of <LastState>
        */        
        qSM_Status_t LastReturnStatus;
        /* Parent: (Read/Write)  
        The parent FSM
        */
        void *Parent;
    }_qSM_PublicData_t;

    typedef _qSM_PublicData_t* qSM_Handler_t;    
    typedef qSM_Status_t (*qSM_State_t)(qSM_Handler_t arg); 
    typedef void (*qSM_SubState_t)(qSM_Handler_t arg); 
    typedef qBool_t (*qSM_SignalAction)( qSM_Handler_t arg );

    typedef struct{
        qSM_State_t xCurrentState;
        qSM_Signal_t Signal;
        qSM_State_t xNextState;
        qSM_SignalAction SignalAction;
    }qSM_Transition_t;

    typedef struct{
        /*This data should be handled only using the provided API*/
        struct _qSM_TransitionTable_Private_s{
            size_t NumberOfEntries;  
            qSM_Transition_t *Transitions;  
        }qPrivate;
    }qSM_TransitionTable_t;
    
    typedef struct _qSM_s{
        /*This data should be handled only using the provided API*/
        struct _qSM_Private_s{
            void (*Failure)(_qSM_Handler_t arg);
            void (*Success)(_qSM_Handler_t arg);
            void (*Unexpected)(_qSM_Handler_t arg);  
            void (*BeforeAnyState)(_qSM_Handler_t arg);
            qSM_TransitionTable_t *TransitionTable;
            void *Owner;
            _qSM_PublicData_t xPublic;
            qQueue_t SignalQueue;
            struct{
                struct _qSM_s *head, *next;
                qSM_State_t rootState;
            }Composite;
        }qPrivate;
    }qSM_t;

    #define QSM_SIGNAL_RANGE_MIN    ( (qSM_Signal_t)0u )
    #define QSM_SIGNAL_RANGE_MAX    ( (qSM_Signal_t)0xFFFFFFFCu )
    
    #define QSM_SIGNAL_NONE    ( (qSM_Signal_t)0xFFFFFFFFu )
    #define QSM_SIGNAL_ENTRY   ( (qSM_Signal_t)0xFFFFFFFEu )
    #define QSM_SIGNAL_EXIT    ( (qSM_Signal_t)0xFFFFFFFDu )

    typedef enum{ /*FSM Attribute Flags definition*/
        qSM_RESTART,                        /*< Restart the FSM. */
        qSM_CLEAR_STATE_FIRST_ENTRY_FLAG,   /*< Clear the entry flag for the current state if the NextState field doesn't change. */
        qSM_FAILURE_STATE,                  /*< Set the Failure State. */
        qSM_SUCCESS_STATE,                  /*< Set the Success State. */
        qSM_UNEXPECTED_STATE,               /*< Set the Unexpected State. */
        qSM_BEFORE_ANY_STATE,               /*< Set the state executed before any state. */        
        qSM_UNINSTALL_TRANSTABLE            /*< To unistall the transition table if available*/      
    }qFSM_Attribute_t; 

    qBool_t qStateMachine_Setup( qSM_t * const obj, qSM_State_t InitState, qSM_SubState_t SuccessState, qSM_SubState_t FailureState, qSM_SubState_t UnexpectedState, qSM_SubState_t BeforeAnyState );
    qSM_Status_t qStateMachine_Run( qSM_t * const obj, void *Data );
    void qStateMachine_Attribute( qSM_t * const obj, const qFSM_Attribute_t Flag , qSM_State_t  s, qSM_SubState_t subs );
    qBool_t qStateMachine_SignalQueueSetup( qSM_t * const obj, qSM_Signal_t *AxSignals, size_t MaxSignals );

    qBool_t qStateMachine_TransitionTableInstall( qSM_t * const obj, qSM_TransitionTable_t *table, qSM_Transition_t *entries, size_t NoOfEntries );
    qBool_t qStateMachine_SweepTransitionTable( qSM_t * const obj );
    qBool_t qStateMachine_SendSignal( qSM_t * const obj, qSM_Signal_t signal, qBool_t isUrgent );
    qSM_Handler_t qStateMachine_Get_Handler( qSM_t * const obj );
    void qStateMachine_Set_Parent( qSM_t * const Child, qSM_t * const Parent );
    
qBool_t qStateMachine_Set_CompositeState( qSM_t * const parent, qSM_State_t state, qSM_t * const child );

    #ifdef __cplusplus
    }
    #endif

#endif 
