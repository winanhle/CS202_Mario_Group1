# Declaration of Generative AI Usage

## General Information

| Field | Information |
|---|---|
| University | University of Science - Faculty of Information Technology |
| Course | Object-Oriented Programming - CS202 |
| Class - Group | 25A01 - Group 1 |
| Project | Super Mario Bros. |
| AI tools disclosed | OpenAI ChatGPT/Codex, Claude 3.7/Sonnet, and Google Gemini/Antigravity |

### Group members

| Student | Student ID |
|---|---:|
| Nguyen Le Tran | 25125039 |
| Le Quang Nguyen Phuc | 25125070 |
| Le Quynh Anh | 25125071 |
| Dinh Nguyen Hong Anh | 25125078 |

## 1. Declaration Statement

We declare that generative AI tools were used as supporting tools during selected parts of the *Super Mario Bros.* project. AI assistance was used for code review, debugging suggestions, Git integration guidance, selected gameplay and interface features, persistence, English-language editing, diagram planning, and document formatting. Section 3.1 adds the individual usage record supplied by Le Quang Nguyen Phuc for his assigned subsystems.

The group retained responsibility for the project's requirements, architecture, implementation decisions, source-code integration, testing, and submitted results. AI-generated or AI-assisted material was reviewed against the current project source, revised where necessary, built in the group environment, and tested by group members before acceptance. AI output was not treated as authoritative merely because it was generated.

## 2. Scope of AI Assistance

AI tools were used in the following limited roles:

1. Inspecting the existing project structure and identifying relevant classes, managers, state flow, and design-pattern participants.
2. Suggesting implementation and debugging steps for an animated transition from an overworld pipe to a hidden map and back.
3. Assisting with the integration of drifting cloud scenery while preserving existing map-owned grass and bushes.
4. Diagnosing a visual artifact in which a repeated bush sprite crop could overwrite terrain at some camera positions.
5. Reviewing branch and merge state to reduce the risk of overwriting other members' menu, gameplay, UI, map, enemy, item, sound, save/load, and editor work.
6. Organizing and editing the English project report, including class diagrams, design-pattern explanations, design reasoning, testing tables, and PDF formatting.
7. Preparing this transparent declaration of AI usage.

AI tools were not used to make grading decisions, invent student identities, fabricate source-code evidence, or replace the group's responsibility to understand and explain the submitted implementation.

## 3. AI Usage Register

| No. | Task assisted by AI | Representative prompt or request | AI contribution | Human review and verification |
|---:|---|---|---|---|
| 1 | Repository and branch inspection | "Check the newest code on the testing branch and preserve everyone's existing work." | Helped inspect branch status, locate integration points, and identify shared files that required careful merging. | The group reviewed the selected branch, checked diffs, retained the complete existing UI/game flow, and confirmed that unrelated work was not removed. |
| 2 | Hidden-stage pipe animation | "Add an animation in which the player moves through the pipe before entering the hidden stage." | Helped design an explicit `None -> Traveling -> FadeOut -> FadeIn` transition and the associated input/update restrictions. | The group reviewed the implementation, built the project, played the complete entry/exit sequence, and verified control restoration. |
| 3 | Background scenery | "Add grass/bush scenery and clouds to the existing game." | Helped place decorative cloud rendering in the world render order and use camera-aware repetition/parallax. | The group visually checked scrolling stages and retained bushes/grass in the authoritative TMX tile layer. |
| 4 | Visual defect diagnosis | "The grass/bush background sometimes has a bug." | Helped trace intermittent ground overdraw to a sprite-atlas crop that shared pixels with terrain. | The repeated bush crop was removed; the group retested multiple camera positions and confirmed that terrain remained visible. |
| 5 | Git integration guidance | "Check merge status, use the testing branch, and separate commits." | Suggested scoped diffs and logically separated commits for transition and scenery changes. | Group members reviewed commit contents and remained responsible for repository history and any push to the shared remote. |
| 6 | Architecture and pattern analysis | "Create class diagrams and explain the applied design patterns and design reasoning." | Helped organize evidence for State, Mediator, Memento, Factory, Strategy, Command, and observer-style propagation. | Every named class and relationship was cross-checked against the integrated source tree; unsupported claims were revised or qualified. |
| 7 | Report production | "Write the English report in the style of the old report, at least 17 pages, as Markdown and PDF." | Assisted with structure, language editing, tables, diagram layout, and PDF typesetting. | The group remains responsible for the final wording, factual accuracy, and oral explanation of all submitted material. |
| 8 | AI declaration | "Provide a separate AI declaration in Markdown and PDF." | Helped present a concise disclosure, use register, and integrity statement. | Group members reviewed the declaration for an accurate account of AI assistance. |

### 3.1 Additional individual disclosure - Le Quang Nguyen Phuc (25125070)

Assigned subsystems: UI architecture, audio, state machine and menus, Save/Memento persistence, HUD and scoring, career statistics, and achievements.

| Task | Relevant commits | AI tool(s) | Assistance recorded by the student |
|---|---|---|---|
| Settings and dynamic keybindings | `df1c2eb`, `dfac00f`, `214a149`, `fa32405`, `97c7a1d`, `274ebc3`, `24c977b`, `459a7fa`, `5ccc50b`, `36c971a` | Claude 3.7, ChatGPT | Assisted with the settings manager, two-player key rebinding, INI persistence, fullscreen mouse-coordinate mapping, key-conflict handling, and pause overlay behavior. |
| UI controller and main menu | `397b0b4`, `f9ec72f`, `1dac550` | Claude 3.7, Gemini/Antigravity | Assisted with reusable menu navigation, card scaling, menu background behavior, fade transitions, and text readability. |
| HUD, score effects, and leaderboard | `b91bf9b`, `1ddd716`, `ecfbfb9` | Claude 3.7, ChatGPT | Assisted with score popups, initials entry, leaderboard storage, flagpole scoring, the 100-coin extra-life rule, and timer warning effects. |
| Level selection and visual alignment | `69e0502`, `9c8f892`, `f0775cc` | Gemini/Antigravity, Claude 3.7 | Assisted with the level grid, locked-stage display, camera alignment, progression persistence, and multiplayer score attribution. |
| Memento save system, audio, and World 1-3 ending | `dc09e25`, `39b2fb2` | Claude 3.7, Gemini/Antigravity | Assisted with `GameMemento`, `ISaveManager`, save-system separation, audio behavior, ending-sequence timing, Peach presentation, career statistics, and achievements. |

The student states that he specified the intended behavior, reviewed proposed changes, compiled and playtested the relevant builds, and remained responsible for the final implementation of these modules.

## 4. Human Oversight and Validation

The group applied the following review controls:

- AI suggestions were compared with the latest integrated source rather than an isolated or outdated code fragment.
- Existing work from other team members was treated as authoritative and preserved unless an intentional reviewed change was required.
- Generated code or edits were inspected through diffs before acceptance.
- The project was built and executed in the group's development environment.
- Pipe behavior was tested as a complete cycle: trigger, scripted motion, fade-out, map load, fade-in, and restored input.
- Background behavior was checked while moving the camera through multiple repeated positions.
- Design-pattern claims in the report were tied to named classes and current code evidence.
- Factual errors, overstatements, and unsupported claims remained the responsibility of the group to correct.

## 5. Academic Integrity Declaration

We understand that generative AI is a support tool and does not transfer authorship responsibility away from the students. We confirm that:

1. We understand the submitted architecture and source code and can explain the relevant design decisions.
2. We reviewed and validated AI-assisted content before including it in the project or report.
3. We did not knowingly submit fabricated test evidence, references, or implementation claims.
4. We did not use AI output as a substitute for required individual or group learning.
5. We disclosed the material categories of AI assistance used for this submission.
6. If a course policy requires additional evidence or prompt history, we will provide it to the instructors when requested.
