# GitHub Project Automation Guide

This document explains how the GitHub issue templates, GitHub Actions workflows, and GitHub Project fields work together in this repository.
It is written for someone who has never used GitHub before.

## What this system does

When a contributor creates an issue in GitHub, they see a form with fields such as priority and planned iteration.
After the issue is created, automation moves the issue into the correct GitHub Project, fills project fields, and keeps sprint information up to date.

In this repository the automation is split into three parts:

- Issue templates in `.github/ISSUE_TEMPLATE/`
- GitHub Actions workflows in `.github/workflows/`
- GitHub Project settings and secrets

## Main files and what they do

### Issue templates

The issue templates are the forms users see when creating an issue on GitHub.
Each template defines:

- the issue title prefix
- the labels to apply
- the fields the user must fill in
- the order of the fields

Relevant files:

- `.github/ISSUE_TEMPLATE/bug_report.yml`
- `.github/ISSUE_TEMPLATE/feature_request.yml`
- `.github/ISSUE_TEMPLATE/device_request.yml`
- `.github/ISSUE_TEMPLATE/general.yml`
- `.github/ISSUE_TEMPLATE/documentation.yml`

Important fields used by the automation:

- `priority`
- `planned_iteration`

### Workflow: `auto-priority.yml`

File: `.github/workflows/auto-priority.yml`

This workflow runs when an issue is opened or edited.
It reads the text of the issue body, finds the selected priority and planned iteration, and then updates the linked GitHub Project item.

What it does step by step:

1. Reads the issue body.
2. Finds the `Priority` section.
3. Maps the selected priority to a specific Project option ID.
4. Finds or creates the issue item in the GitHub Project.
5. Updates the Project priority field.
6. Reads the `Планируемая итерация` field from the issue.
7. Chooses the current, next, or no iteration depending on the selected value.
8. Updates the Project iteration field.
9. Adds a comment if `Next iteration` was chosen but the next sprint does not exist yet.

Why it matters:

- The issue author does not need to open the project manually.
- The project gets the correct priority immediately.
- The issue is placed into the right sprint.

### Workflow: `auto-move-incomplete-issues.yml`

File: `.github/workflows/auto-move-incomplete-issues.yml`

This workflow runs on a schedule twice a week.
It checks open issues in the project and moves unfinished ones to the next iteration when the current iteration has ended.

What it does step by step:

1. Loads all iterations from the GitHub Project.
2. Loads project items and their iteration field value.
3. Finds open issues that still belong to an ended iteration.
4. Moves them to the next available iteration.
5. Adds a comment to the issue explaining the move.

The comment also shows how many times the issue has already been transferred.
That makes the issue history easier to use in sprint reports.

Why it matters:

- Items do not get lost when a sprint ends.
- The project stays aligned with the current sprint plan.
- The team can keep a stable sprint workflow without manually moving every issue.

### Workflow: `update-iteration-info.yml`

File: `.github/workflows/update-iteration-info.yml`

This workflow updates the text shown inside the issue templates.
It writes the current date and the current sprint dates into the markdown block under the iteration dropdown.

What it does step by step:

1. Reads the current date.
2. Reads the iteration configuration from the GitHub Project.
3. Finds the current iteration by comparing dates.
4. Builds a short text block with the current date and sprint period.
5. Rewrites the markdown block in every issue template.
6. Commits the changes back to the repository.

Important limitation:

- GitHub issue templates cannot execute JavaScript while the issue form is rendered.
- The dates cannot update live inside the form by themselves.
- The closest practical solution is a workflow that rewrites the templates on a schedule.

### Workflow: `generate-sprint-report.yml`

File: `.github/workflows/generate-sprint-report.yml`

This workflow generates a filled sprint report from GitHub Project data and issue comments.
It writes the result to `docs/project/sprint-report.generated.md`.

What it does step by step:

1. Reads the project iteration configuration.
2. Chooses the most recently completed sprint, or the active one if no sprint is completed yet.
3. Loads project items and issue comments.
4. Detects issues that were transferred out of the sprint.
5. Builds a markdown report with completed work, moved work, transfer counts, and metrics.
6. Writes the generated markdown file into the repository.
7. Commits the report if it changed.

Why it matters:

- The report is filled automatically from live project data.
- Transfer history is visible without manual copying.
- The file can be used as the base for the final human-edited sprint review.

## Secrets and variables

These workflows expect GitHub repository secrets.

### `AUTOMATISATION_PROJECT`

The GitHub token used by `actions/github-script`.
It must have permission to read and update the repository project and create issue comments.

### `PROJECT_ID`

The ID of the GitHub Project V2 board.
The workflows use it to find project items and iteration configuration.

### `PRIORITY_FIELD_ID`

The project field ID for the priority field.
This is the field that receives the selected priority from the issue template.

### `PRIORITY_LOW_ID`
### `PRIORITY_MEDIUM_ID`
### `PRIORITY_HIGH_ID`
### `PRIORITY_CRITICAL_ID`
### `PRIORITY_LONG_TERM_PLANS_ID`

These are the option IDs for the priority single-select field.
Each value in the issue form must map to one project option ID.

### `ITERATION_FIELD_ID`

The project field ID for the iteration field.
The workflow uses it to place the issue into the correct sprint.

## How the priority mapping works

The issue form shows human-readable values, such as:

- Low
- Long-term plans
- Medium
- High
- Critical

Inside the workflow those values are translated into project option IDs.
That translation is required because GitHub Project stores the actual field value by ID, not by display text.

## How the iteration mapping works

The issue form shows these values:

- Current iteration - put into the current sprint
- Next iteration - put into the next sprint
- Backlog - do not assign to an iteration

The workflow interprets them like this:

- Current iteration: assign the issue to the current sprint
- Next iteration: assign the issue to the next sprint
- Backlog: leave the iteration field empty

If the next sprint does not exist yet, the workflow falls back to the current sprint and adds a comment so the user knows what happened.

## How to recreate this setup in a new project

If you want to create the same automation in another repository, follow these steps.

### 1. Create a GitHub Project

Create a Project V2 board in GitHub.
Add the fields you need:

- Priority
- Iteration

### 2. Create issue templates

Add YAML issue forms in `.github/ISSUE_TEMPLATE/`.
Make sure the templates contain the same field IDs that the workflows expect:

- `priority`
- `planned_iteration`

### 3. Collect Project IDs

Open the project settings and copy the field IDs and option IDs.
Store them as repository secrets.

### 4. Add the secrets

Create these secrets in the repository settings:

- `AUTOMATISATION_PROJECT`
- `PROJECT_ID`
- `PRIORITY_FIELD_ID`
- `PRIORITY_LOW_ID`
- `PRIORITY_MEDIUM_ID`
- `PRIORITY_HIGH_ID`
- `PRIORITY_CRITICAL_ID`
- `PRIORITY_LONG_TERM_PLANS_ID`
- `ITERATION_FIELD_ID`

### 5. Add the workflows

Copy the workflow files into `.github/workflows/`.
Then commit the changes and push them to the repository.

### 6. Test the system

Create a test issue from each template and verify that:

- the issue gets the correct priority in the project
- the issue gets the correct iteration
- the workflow creates a comment when needed
- the sprint info block updates after the scheduled workflow runs

## What each file is responsible for

- `.github/ISSUE_TEMPLATE/*.yml`: user input forms
- `.github/workflows/auto-priority.yml`: sync issue fields into the project
- `.github/workflows/auto-move-incomplete-issues.yml`: move unfinished work forward
- `.github/workflows/update-iteration-info.yml`: keep template sprint info current

## Practical notes

- The dynamic sprint block inside the templates is not truly live.
- It is updated by a scheduled workflow.
- If the project iteration dates change, the workflow must run again to refresh the text.
- Keep the secret values private.
- Do not store field IDs or tokens inside the repository files.
- Transfer history is stored in the issue comments, not in a separate JSON file.
- The generated sprint report is written to `docs/project/sprint-report.generated.md`.

## Quick mental model

Think of the system like this:

1. The issue template asks the user for priority and iteration.
2. The workflow reads those answers.
3. The workflow updates the GitHub Project.
4. Another workflow keeps sprint information and unfinished work in sync.

That is the whole automation loop.
